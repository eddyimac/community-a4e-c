#include "Airframe.h"
#include <algorithm>
Skyhawk::Airframe::Airframe( AircraftState& state, Input& controls, Engine2& engine):
	m_state(state),
	m_controls(controls),
	m_engine(engine)
{
	m_integrityElement = new float[(int)Damage::COUNT];
	zeroInit();
}

Skyhawk::Airframe::~Airframe()
{
	delete[] m_integrityElement;
}

void Skyhawk::Airframe::printDamageState()
{
	printf( "===========================================\n" );

	printf( "            ||\n" );
	printf( "            ||\n" );
	printf( "%.1lf %.1lf %.1lf || %.1lf %.1lf %.1lf\n", 
		DMG_ELEM(Damage::WING_L_OUT), 
		DMG_ELEM(Damage::WING_L_CENTER), 
		DMG_ELEM(Damage::WING_L_IN),
		DMG_ELEM( Damage::WING_R_IN ),
		DMG_ELEM( Damage::WING_R_CENTER ),
		DMG_ELEM( Damage::WING_R_OUT ) );

	printf( "        %.1f || %.1f\n", DMG_ELEM(Damage::AILERON_L), DMG_ELEM(Damage::AILERON_R) );
	printf( "            ||\n" );
	printf( "        %.1f || %.1f\n", DMG_ELEM( Damage::STABILIZATOR_L ), DMG_ELEM( Damage::STABILIZATOR_R ) );
	printf( "        %.1f || %.1f\n", DMG_ELEM( Damage::ELEVATOR_L ), DMG_ELEM( Damage::ELEVATOR_R ) );
	printf( "            %.1f\n", getVertStabDamage() );
	printf( "            %.1f\n", getRudderDamage() );
}

void Skyhawk::Airframe::resetDamage()
{
	for ( int i = 0; i < (int)Damage::COUNT; i++ )
	{
		m_integrityElement[i] = 1.0f;
	}
}

//Seriously need to set EVERY VARIABLE to zero (or approriate value if zero causes singularity) in the constructor and 
//in this function. Otherwise Track's become unusable because of the butterfly effect.
void Skyhawk::Airframe::zeroInit()
{
	m_gearLPosition = 0.0;
	m_gearRPosition = 0.0;
	m_gearNPosition = 0.0;

	m_flapsPosition = 0.0;
	m_spoilerPosition = 0.0;
	m_speedBrakePosition = 0.0;
	m_hookPosition = 0.0;
	m_slatLPosition = 0.0;
	m_slatRPosition = 0.0;

	m_aileronLeft = 0.0;
	m_aileronRight = 0.0;
	m_elevator = 0.0;
	m_rudder = 0.0;

	m_selected = Tank::INTERNAL;

	for ( int i = 0; i < Tank::DONT_TOUCH; i++ )
	{
		m_fuelPrev[i] = 0.0;
	}

	resetDamage();

	m_mass = 1.0;


	m_catapultState = CatapultState::OFF_CAT;
	m_catStateSent = false;
	m_catMoment = 0.0;
	m_angle = 0.0;
	m_noseCompression = 0.0;

	m_damageStack.clear();
}

void Skyhawk::Airframe::coldInit()
{
	zeroInit();
}

void Skyhawk::Airframe::hotInit()
{
	zeroInit();
}

void Skyhawk::Airframe::airborneInit()
{
	zeroInit();
}

void Skyhawk::Airframe::airframeUpdate(double dt)
{

	if (m_controls.hook())
	{
		m_hookPosition += dt / m_hookExtendTime;
		m_hookPosition = std::min(m_hookPosition, 1.0);
	}
	else
	{
		m_hookPosition -= dt / m_hookExtendTime;
		m_hookPosition = std::max(m_hookPosition, 0.0);
	}

	double dm = m_engine.getFuelFlow()*dt;

	int totalExt = 0;
	for (int i = Tank::INTERNAL; i < Tank::DONT_TOUCH; i++)
	{
		//m_fuelPrev[i] = m_fuel[i];
		if (m_fuel[i] > 10.0 && i > Tank::INTERNAL)
		{
			totalExt++;
		}
	}

	double sub_dm = totalExt > 0 ? dm / (double)totalExt : dm;
	for (int i = Tank::INTERNAL; i < Tank::DONT_TOUCH; i++)
	{
		if (i == Tank::INTERNAL)
		{
			if (!totalExt)
			{
				m_fuel[i] -= dm;
				break;
			}
		}
		else if (m_fuel[i] > 10.0)
		{
			m_fuel[i] -= sub_dm;
		}
	}

	//printf("LEFT: %lf, CENTRE: %lf, RIGHT: %lf, INTERNAL: %lf\n", m_fuel[Tank::LEFT_EXT], m_fuel[Tank::CENTRE_EXT], m_fuel[Tank::RIGHT_EXT], m_fuel[Tank::INTERNAL]);
	m_engine.setHasFuel(m_fuel[Tank::INTERNAL] > 50.0);
	
	m_elevator = setElevator(dt);
	m_aileronLeft = setAileron(dt);
	m_aileronRight = -m_aileronLeft;
	m_rudder = setRudder(dt);


	if (m_catapultState == ON_CAT_NOT_READY && m_engine.getRPMNorm() > 0.9)
	{
		m_catapultState = ON_CAT_READY;
	}

	if (m_catapultState != OFF_CAT)
	{
		//m_catMoment = pow((c_catAngle - m_state.getAngle().z)*60.0, 3.0) * c_catConstrainingForce;
		double desiredMoment = /*-500.0 * pow(8.0*std::max(1.0 - m_noseCompression, 0.0), 3.0)*/ - std::max( m_state.getLocalAcceleration().x * c_maxCatMoment * 0.03, 0.0 );

		m_catMomentVelocity += (desiredMoment - m_catMoment) * dt * 1.0;

		m_catMoment += (desiredMoment - m_catMoment) * dt * 10.0 + m_catMomentVelocity * dt * 5.0 ;

		printf( "Desired Moment %lf, Moment %lf, Velocity: %lf\n", desiredMoment, m_catMoment, m_catMomentVelocity );

		//m_catMoment = std::max( std::min(m_catMoment, 0.0), -c_maxCatMoment );
	}
	else
	{
		m_catMoment = 0.0;
		m_catMomentVelocity = 0.0;
	}
	//printf("Cat Moment: %lf\n", m_catMoment);

	m_catStateSent = false;

	//printDamageState();
}
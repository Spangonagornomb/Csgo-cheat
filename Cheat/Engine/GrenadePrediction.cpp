#include "GrenadePrediction.h"
#include"../Hacks/Setup.h"
//#include "Engine\Render.h"
#include <vector>


void grenade_prediction::Tick(int buttons)
{
	bool in_attack = buttons & IN_ATTACK;
	bool in_attack2 = buttons & IN_ATTACK2;

	act = (in_attack && in_attack2) ? ACT_LOB :
		(in_attack2) ? ACT_DROP :
		(in_attack) ? ACT_THROW :
		ACT_NONE;
}

void grenade_prediction::View()
{
	CBaseEntity* local = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());

	if (local && !local->IsDead() )
	{

		if (CGlobal::GWeaponType == WEAPON_TYPE_GRENADE && (ACT)act != ACT_NONE)
		{
			type = CGlobal::GWeaponID;
			Simulate();
		}
		else
		{
			type = 0;
		}
	}
}

void grenade_prediction::Paint()
{

	if ((type) && path.size() > 1)
	{
		static vector<Vector> TempPath;
		static vector<Vector> CollisTempPath;
		if (Simulated)
		{
			TempPath = path;
			//CollisTempPath = CollisionsPath;
		}
		Simulated = false;

		if (TempPath.size() > 0)
		{
			Vector nadeStart, nadeEnd;

			if (GP_Esp && GP_Render)
			{
				Color Color  = GP_Esp->GranadePredictionColor;
				Vector prev = TempPath[0];

				for (auto it = TempPath.begin(), end = TempPath.end(); it != end; ++it)
				{

					if (CGlobal::WorldToScreen(prev, nadeStart) && CGlobal::WorldToScreen(*it, nadeEnd))
						GP_Render->DrawLine(nadeStart.x, nadeStart.y, nadeEnd.x, nadeEnd.y, Color, 2.f);

					prev = *it;
				}

				if (CGlobal::WorldToScreen(prev, nadeEnd))
					GP_Render->DrawRing(nadeEnd.x, nadeEnd.y, 10, 22, Color::Red(), 2.f);
			}
		/*	if(type == WEAPON_HEGRENADE && Settings::Esp::esp_WorldGrenadeTrajRadius)
				Client::g_pRender->Circle3D(prev, 36, 341, Color::Red());*/

			//if (CollisTempPath.size() > 0)
			//	for (auto it = TempPath.begin(), end = TempPath.end(); it != end; ++it)
			//		if (WorldToScreen(*it, nadeEnd))
			//			GP_IRender->render_Box(nadeEnd.x - 3, nadeEnd.y - 3, 6, 6, Color::Red().GetImColor());

		}
		
	}
}
static const constexpr auto PIRAD = 0.01745329251f;
void angle_vectors2(const Vector &angles, Vector *forward, Vector *right, Vector *up)
{
	float sr, sp, sy, cr, cp, cy;

	sp = static_cast<float>(sin(double(angles.x) * PIRAD));
	cp = static_cast<float>(cos(double(angles.x) * PIRAD));
	sy = static_cast<float>(sin(double(angles.y) * PIRAD));
	cy = static_cast<float>(cos(double(angles.y) * PIRAD));
	sr = static_cast<float>(sin(double(angles.z) * PIRAD));
	cr = static_cast<float>(cos(double(angles.z) * PIRAD));

	if (forward)
	{
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right)
	{
		right->x = (-1 * sr*sp*cy + -1 * cr*-sy);
		right->y = (-1 * sr*sp*sy + -1 * cr*cy);
		right->z = -1 * sr*cp;
	}

	if (up)
	{
		up->x = (cr*sp*cy + -sr * -sy);
		up->y = (cr*sp*sy + -sr * cy);
		up->z = cr * cp;
	}
}
void grenade_prediction::Setup(Vector& vecSrc, Vector& vecThrow, Vector viewangles)
{
	Vector angThrow = viewangles;
	CBaseEntity* local = (CBaseEntity*)I::EntityList()->GetClientEntity(I::Engine()->GetLocalPlayer());
	float pitch = angThrow.x;

	if (!local)
		return;

	if (pitch <= 90.0f)
	{
		if (pitch<-90.0f)
		{
			pitch += 360.0f;
		}
	}
	else
	{
		pitch -= 360.0f;
	}
	float a = pitch - (90.0f - fabs(pitch)) * 10.0f / 90.0f;
	angThrow.x = a;

	// Gets ThrowVelocity from weapon files
	// Clamped to [15,750]
	float flVel = 750.0f * 0.9f;

	// Do magic on member of grenade object [esi+9E4h]
	// m1=1  m1+m2=0.5  m2=0
	static const float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	float b = power[act];
	// Clamped to [0,1]
	b = b * 0.7f;
	b = b + 0.3f;
	flVel *= b;

	Vector vForward, vRight, vUp;
	angle_vectors2(angThrow, &vForward, &vRight, &vUp); //angThrow.ToVector(vForward, vRight, vUp);

	vecSrc = local->GetEyePosition();
	float off = (power[act] * 12.0f) - 12.0f;
	vecSrc.z += off;

	// Game calls UTIL_TraceHull here with hull and assigns vecSrc tr.endpos
	trace_t tr;
	Vector vecDest = vecSrc;
	vecDest += vForward * 22.0f; //vecDest.MultAdd(vForward, 22.0f);

	TraceHull(vecSrc, vecDest, tr);

	// After the hull trace it moves 6 units back along vForward
	// vecSrc = tr.endpos - vForward * 6
	Vector vecBack = vForward; vecBack *= 6.0f;
	vecSrc = tr.endpos;
	vecSrc -= vecBack;

	// Finally calculate velocity
	vecThrow = local->GetVelocity(); vecThrow *= 1.25f;
	vecThrow += vForward * flVel; //	vecThrow.MultAdd(vForward, flVel);
}

void grenade_prediction::Simulate()
{
	Vector vecSrc, vecThrow;
	Vector angles; I::Engine()->GetViewAngles(angles);
	Setup(vecSrc, vecThrow, angles);

	float interval = I::GlobalVars()->interval_per_tick;

	// Log positions 20 times per sec
	int logstep = static_cast<int>(0.05f / interval);
	int logtimer = 0;

	if (!Simulated)
	{
		path.clear();
		CollisionsPath.clear();
		for (unsigned int i = 0; i < path.max_size() - 1; ++i)
		{

			if (i > 500)
				break;

			if (!logtimer)
				path.push_back(vecSrc);
			bool Detonate = false;

			int s = Step(vecSrc, vecThrow, i, interval, Detonate);
			if ((s & 1)) break;

			if (Detonate)
				break;

			// Reset the log timer every logstep OR we bounced
			if ((s & 2) || logtimer >= logstep) logtimer = 0;
			else ++logtimer;
		}
		path.push_back(vecSrc);
		Simulated = true;
	}
}

int grenade_prediction::Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval, bool &Detonated)
{

	// Apply gravity
	Vector move;
	AddGravityMove(move, vecThrow, interval, false);

	// Push entity
	trace_t tr;
	PushEntity(vecSrc, move, tr);

	int result = 0;
	// Check ending conditions
	if (CheckDetonate(vecThrow, tr, tick, interval))
	{
		Detonated = true;
		result |= 1;
	}

	// Resolve collisions
	if (tr.fraction != 1.0f)
	{
		result |= 2; // Collision!
		ResolveFlyCollisionCustom(tr, vecThrow, interval);
	}

	if(vecThrow != tr.endpos)
	//	CollisionsPath.push_back(tr.endpos);

	// Set new position
	vecSrc = tr.endpos;

	return result;
}


bool grenade_prediction::CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval)
{
	switch (type)
	{
	case WEAPON_SMOKEGRENADE:
	case WEAPON_DECOY:
		// Velocity must be <0.1, this is only checked every 0.2s
		if (vecThrow.Length2D()<0.1f)
		{
			int det_tick_mod = static_cast<int>(0.2f / interval);
			return !(tick%det_tick_mod);
		}
		return false;

	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
		// Detonate when hitting the floor
		if (tr.fraction != 1.0f && tr.plane.normal.z>0.7f)
			return true;
		// OR we've been flying for too long

	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
		// Pure timer based, detonate at 1.5s, checked every 0.2s
		return static_cast<float>(tick)*interval>1.5f && !(tick%static_cast<int>(0.2f / interval));

	default:
		return false;
	}
}

void grenade_prediction::TraceHull(Vector& src, Vector& end, trace_t& tr)
{

	Ray_t ray;
	ray.Init(src, end, Vector(-2.0f, -2.0f, -2.0f), Vector(2.0f, 2.0f, 2.0f));

	CTraceFilterWorldAndPropsOnly filter;
	//filter.SetIgnoreClass("BaseCSGrenadeProjectile");
	//filter.bShouldHitPlayers = false;
	

	I::EngineTrace()->TraceRay(ray, MASK_SOLID/*0x200400B*/, &filter, &tr);
}

void grenade_prediction::AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground)
{

	Vector basevel(0.0f, 0.0f, 0.0f);

	move.x = (vel.x + basevel.x) * frametime;
	move.y = (vel.y + basevel.y) * frametime;

	if (onground)
	{
		move.z = (vel.z + basevel.z) * frametime;
	}
	else
	{
		// Game calls GetActualGravity( this );
		float gravity = 800.0f * 0.4f;

		float newZ = vel.z - (gravity * frametime);
		move.z = ((vel.z + newZ) / 2.0f + basevel.z) * frametime;

		vel.z = newZ;
	}
}

void grenade_prediction::PushEntity(Vector& src, const Vector& move, trace_t& tr)
{

	Vector vecAbsEnd = src;
	vecAbsEnd += move;

	// Trace through world
	TraceHull(src, vecAbsEnd, tr);
}

void grenade_prediction::ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval)
{

	// Calculate elasticity
	float flSurfaceElasticity = 1.0;  // Assume all surfaces have the same elasticity
	float flGrenadeElasticity = 0.45f; // GetGrenadeElasticity()
	float flTotalElasticity = flGrenadeElasticity * flSurfaceElasticity;
	if (flTotalElasticity>0.9f) flTotalElasticity = 0.9f;
	if (flTotalElasticity<0.0f) flTotalElasticity = 0.0f;

	// Calculate bounce
	Vector vecAbsVelocity;
	int s = PhysicsClipVelocity(vecVelocity, tr.plane.normal, vecAbsVelocity, 2.0f);

	//if(s & 2)
		//CollisionsPath.push_back(tr.endpos);
	vecAbsVelocity *= flTotalElasticity;

	// Stop completely once we move too slow
	float flSpeedSqr = vecAbsVelocity.LengthSqr();
	static const float flMinSpeedSqr = 20.0f * 20.0f; // 30.0f * 30.0f in CSS
	if (flSpeedSqr<flMinSpeedSqr)
	{
		//vecAbsVelocity.Zero();
		vecAbsVelocity.x = 0.0f;
		vecAbsVelocity.y = 0.0f;
		vecAbsVelocity.z = 0.0f;
	}

	// Stop if on ground
	if (tr.plane.normal.z>0.7f)
	{
		vecVelocity = vecAbsVelocity;
		vecAbsVelocity *= ((1.0f - tr.fraction) * interval); //vecAbsVelocity.Mult((1.0f - tr.fraction) * interval);
		PushEntity(tr.endpos, vecAbsVelocity, tr);
	}
	else
	{
		
		vecVelocity = vecAbsVelocity;
	}
}

int grenade_prediction::PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce)
{
	static const float STOP_EPSILON = 0.1f;

	float    backoff;
	float    change;
	float    angle;
	int        i, blocked;

	blocked = 0;

	angle = normal[2];

	if (angle > 0)
	{
		blocked |= 1;        // floor
	}
	if (!angle)
	{
		blocked |= 2;        // step	
	}

	backoff = in.Dot(normal) * overbounce;

	for (i = 0; i<3; i++)
	{
		change = normal[i] * backoff;
		out[i] = in[i] - change;
		if (out[i] > -STOP_EPSILON && out[i] < STOP_EPSILON)
		{
			out[i] = 0;
		}
	}

	return blocked;
} 
#pragma once
#include"../Main.h"


enum ACT
{
	ACT_NONE,
	ACT_THROW,
	ACT_LOB,
	ACT_DROP,
};

class grenade_prediction
{
private:
	std::vector<Vector> path;
	std::vector<Vector> CollisionsPath;

	int type = 0;
	int act = 0;

	void Setup(Vector& vecSrc, Vector& vecThrow, Vector viewangles);
	void Simulate();

	int Step(Vector& vecSrc, Vector& vecThrow, int tick, float interval, bool &Detonated);
	bool CheckDetonate(const Vector& vecThrow, const trace_t& tr, int tick, float interval);

	void TraceHull(Vector& src, Vector& end, trace_t& tr);
	void AddGravityMove(Vector& move, Vector& vel, float frametime, bool onground);
	void PushEntity(Vector& src, const Vector& move, trace_t& tr);
	void ResolveFlyCollisionCustom(trace_t& tr, Vector& vecVelocity, float interval);
	int PhysicsClipVelocity(const Vector& in, const Vector& normal, Vector& out, float overbounce);

	bool Simulated = false;

public:
	void Tick(int buttons);
	void View();
	void Paint();

	static grenade_prediction& Get()
	{
		static grenade_prediction obj;
		return obj;
	}
};   
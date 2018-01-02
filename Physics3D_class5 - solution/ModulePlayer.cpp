#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "Primitive.h"
#include "PhysVehicle3D.h"
#include "PhysBody3D.h"
#include "ModuleSceneIntro.h"


ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled), vehicle(NULL)
{
	name.create("player");

	turn = acceleration = brake = 0.0f;
}

bool ModulePlayer::Init(pugi::xml_node& node) 
{
	bool ret = true;

	return ret;
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	VehicleInfo car;

	car.parts = 0;
	car.parts_size = new vec3[car.parts];
	car.parts_offset = new vec3[car.parts];

	// Car properties ----------------------------------------
	//car.parts_size[0].Set(0.5f, 2, 4);
	//car.parts_offset[0].Set(10, 10, 0);
	//car.parts_size[1].Set(2, 2, 4);
	//car.parts_offset[1].Set(0, 1.5, 0);
	//car.parts_size[2].Set(2, 2, 4);
	//car.parts_offset[2].Set(0, 1.5, 0);
	//car.parts_size[3].Set(2, 2, 4);
	//car.parts_offset[3].Set(0, 1.5, 0);
	car.chassis_size.Set(2, 1, 4);
	car.chassis_offset.Set(0, 1, 0);

	car.mass = 500.0f;
	car.suspensionStiffness = 15.88f;
	car.suspensionCompression = 0.83f;
	car.suspensionDamping = 0.88f;
	car.maxSuspensionTravelCm = 1000.0f;
	car.frictionSlip = 100.5;
	car.maxSuspensionForce = 6000.0f;

	// Wheel properties ---------------------------------------
	float connection_height = 1.2f;
	float wheel_radius = 0.5f;
	float wheel_width = 0.5f;
	float suspensionRestLength = 1.2f;

	// Don't change anything below this line ------------------

	float half_width = car.chassis_size.x*0.5f;
	float half_length = car.chassis_size.z*0.5f;
	
	vec3 direction(0,-1,0);
	vec3 axis(-1,0,0);
	
	car.num_wheels = 4;
	car.wheels = new Wheel[4];

	// FRONT-LEFT ------------------------
	car.wheels[0].connection.Set(half_width - 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[0].direction = direction;
	car.wheels[0].axis = axis;
	car.wheels[0].suspensionRestLength = suspensionRestLength;
	car.wheels[0].radius = wheel_radius;
	car.wheels[0].width = wheel_width;
	car.wheels[0].front = true;
	car.wheels[0].drive = true;
	car.wheels[0].brake = false;
	car.wheels[0].steering = true;

	// FRONT-RIGHT ------------------------
	car.wheels[1].connection.Set(-half_width + 0.3f * wheel_width, connection_height, half_length - wheel_radius);
	car.wheels[1].direction = direction;
	car.wheels[1].axis = axis;
	car.wheels[1].suspensionRestLength = suspensionRestLength;
	car.wheels[1].radius = wheel_radius;
	car.wheels[1].width = wheel_width;
	car.wheels[1].front = true;
	car.wheels[1].drive = true;
	car.wheels[1].brake = false;
	car.wheels[1].steering = true;

	// REAR-LEFT ------------------------
	car.wheels[2].connection.Set(half_width - 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[2].direction = direction;
	car.wheels[2].axis = axis;
	car.wheels[2].suspensionRestLength = suspensionRestLength;
	car.wheels[2].radius = wheel_radius;
	car.wheels[2].width = wheel_width;
	car.wheels[2].front = false;
	car.wheels[2].drive = false;
	car.wheels[2].brake = true;
	car.wheels[2].steering = false;

	// REAR-RIGHT ------------------------
	car.wheels[3].connection.Set(-half_width + 0.3f * wheel_width, connection_height, -half_length + wheel_radius);
	car.wheels[3].direction = direction;
	car.wheels[3].axis = axis;
	car.wheels[3].suspensionRestLength = suspensionRestLength;
	car.wheels[3].radius = wheel_radius;
	car.wheels[3].width = wheel_width;
	car.wheels[3].front = false;
	car.wheels[3].drive = false;
	car.wheels[3].brake = true;
	car.wheels[3].steering = false;

	vehicle = App->physics->AddVehicle(car);
	position = { 0, 12, 10 };
	vehicle->SetPos(position.x, position.y, position.z);
	
	Sphere ballon;
	ballon.radius = 1.0f;
	ballon.SetPos(0, 16, 10);

	sphere = App->physics->AddBody(ballon);
	float x, y, z;
	vehicle->GetPos(x, y, z);
	float i, j, k;
	sphere->GetPos(i, j, k);
	//App->physics->AddConstraintP2P(*(PhysBody3D*)(vehicle->body), *sphere, vec3(x,y,z), vec3(i, j, k));

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	return true;
}

// Update: draw background
update_status ModulePlayer::Update(float dt)
{
	turn = acceleration = brake = 0.0f;

	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
	{
		acceleration = MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		if (turn < TURN_DEGREES)
			turn += TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		if(turn > -TURN_DEGREES)
			turn -= TURN_DEGREES;
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
	{
		acceleration = -MAX_ACCELERATION;
	}

	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		vehicle->Push(0, 5000.0f, 0);
	}

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		brake = BRAKE_POWER;
	}

	if (App->input->GetKey(SDL_SCANCODE_Z) == KEY_REPEAT)
	{
			vehicle->body->setLinearVelocity(btVector3(0, 0, 0));
			vehicle->body->setAngularVelocity(btVector3(0, 0, 0));
			vehicle->SetTransform(IdentityMatrix.M);
		
		switch (App->scene_intro->checkpoints_index)
		{
		case 0:
			vehicle->SetPos(0, 0, 0);
			break;
		case 1:
			vehicle->SetPos(0, 20, 0);
			break;
		case 2:
			vehicle->SetPos(0, 10, 0);
			break;
			//etc
		}
	}

	if (vehicle->GetKmh() > 50)
		acceleration = 0;


	if (vehicle->GetKmh() > 0)
		acceleration -= 100;

	if (speedupZ)
	{
		vehicle->Push(0, 0, 300.0f);
		speedupZ = false;
	}

	else if (speedupZnegative)
	{
		vehicle->Push(0, 0, -300.0f);
		speedupZnegative = false;
	}

	if (speedupX)
	{
		vehicle->Push(300.0f, 0, 0);
		speedupX = false;
	}

	vehicle->ApplyEngineForce(acceleration);

	vehicle->Turn(turn);
	vehicle->Brake(brake);
	vehicle->Render();

	vehicle->GetPos(position.x, position.y, position.z);
	position;
	char title[80];
	sprintf_s(title, "%.1f Km/h", vehicle->GetKmh());
	App->window->SetTitle(title);

	return UPDATE_CONTINUE;
}

/*
void ModuleSceneIntro::OnCollision(PhysBody3D* body1, PhysBody3D* body2)
{
	if (body1 == sensor && body2 == (PhysBody3D*)App->player->vehicle) {
		App->player->speedupZ = true;
	}

	else if (body1 == sensor2 && body2 == (PhysBody3D*)App->player->vehicle) {
		App->player->speedupZ = true;
	}

	else if (body1 == sensor3 && body2 == (PhysBody3D*)App->player->vehicle) {
		App->player->speedupZ = true;
	}

	else if (body1 == sensor4 && body2 == (PhysBody3D*)App->player->vehicle) {
		App->player->speedupX = true;
	}

	else if (body1 == sensor5 && body2 == (PhysBody3D*)App->player->vehicle) {
		App->player->speedupZnegative = true;
	}


	else if (body1) {
		checkpoints_index = 1;
	}
}
*/

/*
// Sensors
s.size = vec3(4, 2, 8); // First sensor, dont touch
s.SetPos(0, 1.5f, 3); // First sensor, dont touch

g.size = vec3(4, 2, 8); // Second sensor, dont touch
g.SetPos(60, 1.5f, 158); // Second sensor, dont touch

h.size = vec3(10, 2, 8); // Third sensor, need to be fixed
h.SetPos(60, 1.5f, 240); // Third sensor, need to be fixed

t.size = vec3(14, 2, 7); // forth sensor, need to be fixed
t.SetPos(125, -4, 369); // forth sensor, need to be fixed

n.size = vec3(32, 2, 9); // fifth sensor, need to be fixed
n.SetPos(277, 0.5f, 287); // fifth sensor, need to be fixed
*/
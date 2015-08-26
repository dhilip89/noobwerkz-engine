#pragma once

#include <stack>
#include <string>
#include <tuple>

#include "Config.hpp"

#include "Graphics.hpp"
#include "MathFuncs.hpp"
#include "VoxelWorld.hpp"

#include <boost/variant.hpp>
#include "ShaderVariant.hpp"

#include "TriplanarGradientMap.hpp"
#include "BasicRenderer.hpp"
#include "TransformHelper.hpp"

#include "Actor.hpp"
#include "SkeletalAnim.hpp"
#include "Model.hpp"
#include "TransformHelper.hpp"
#include "CharacterController.hpp"
#include "PhysicsContacts.hpp"

#include "reactphysics3d.h"

namespace noob
{
	class stage
	{
		public:
			stage() : world(rp3d::Vector3(0.0, -9.81, 0.0), rp3d::decimal(1.0/60.0)), paused(false) {}

			bool init();
			
			void update(double dt);
			
			void draw() const;
			void draw(const std::shared_ptr<noob::actor>& a) const;
			void pause() { paused = true; }
			void start() { paused = false; }

			std::shared_ptr<noob::actor> make_actor(const std::string& name, const std::shared_ptr<noob::model>&, const std::shared_ptr<noob::skeletal_anim>&, const std::shared_ptr<noob::prepared_shaders::info>&, const noob::mat4& transform, float mass = 2.0, float width = 0.25, float height = 1.0, float max_speed = 5.0);

			// Loads a serialized model (from cereal binary)
			bool add_model(const std::string& name, const std::string& filename);
			bool add_model(const std::string& name, const noob::basic_mesh&);

			bool add_skeleton(const std::string& name, const std::string& filename);
			
			void set_shader(const std::string& name, const noob::prepared_shaders::info& info);

			std::weak_ptr<noob::actor> get_actor(const std::string& name) const;
			std::weak_ptr<noob::prepared_shaders::info> get_shader(const std::string& name) const;
			std::weak_ptr<noob::model> get_model(const std::string& name) const;
			std::weak_ptr<noob::skeletal_anim> get_skeleton(const std::string& name) const;
			
			

			//rp3d::CollisionBody* physics_body(float friction, float bouncy, float linear_damping, float angular_damping, noob::mat4 world_transform = noob::identity_mat4(), body_type type = noob::stage::body_type::DYNAMIC, bool gravity = true);
			
			void add_sphere_to_body(rp3d::CollisionBody* body, float radius, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_box_to_body(rp3d::CollisionBody* body, float width, float height, float depth, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_cylinder_to_body(rp3d::CollisionBody* body, float radius, float height, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_capsule_to_body(rp3d::CollisionBody* body, float radius, float height,  float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			void add_cone_to_body(rp3d::CollisionBody* body, float radius, float height, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());
			// This one creates a mesh from several convex hulls (ideally created beforehand via the noob::basic_mesh::convex_decomposition() interface.) May bugger up if meshes aren't convex
			void add_mesh_to_body(rp3d::CollisionBody* body, const std::vector<noob::basic_mesh>& mesh, float mass, const noob::mat4& local_transform = noob::identity_mat4(), short collision_mask = 1, short collides_with = std::numeric_limits<short>::max());

			void draw_pose(const std::shared_ptr<noob::skeletal_anim>&);//, const std::string& anim_name, float time);
			
		protected:
			rp3d::DynamicsWorld world;
			noob::physics_event_listener phyz_listener;
			noob::prepared_shaders shaders;
			bool paused;
			// TODO: Bring HACD renderer in line with the rest of the shader types
			noob::triplanar_renderer triplanar_render;
			noob::basic_renderer basic_render;
			noob::mat4 view_mat, projection_mat;

			// For fast access to basic shapes. Test to see difference.
			std::shared_ptr<noob::model> unit_cube, unit_sphere, unit_cylinder, unit_cone;

			std::unordered_map<std::string, std::shared_ptr<noob::actor>> actors;
			std::unordered_map<std::string, std::shared_ptr<noob::prepared_shaders::info>> shader_uniforms;
			std::unordered_map<std::string, std::shared_ptr<noob::model>> models;
			std::unordered_map<std::string, std::shared_ptr<noob::skeletal_anim>> skeletons;
	};
}


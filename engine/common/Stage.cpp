#include "Stage.hpp"
// #include "Shiny.h"
#include "LogObj.hpp"

noob::stage::~stage() noexcept(true) 
{
	delete dynamics_world;
	delete solver;
	delete collision_configuration;
	delete collision_dispatcher;
	delete broadphase;
}

void noob::stage::init() noexcept(true) 
{
	broadphase = new btDbvtBroadphase();
	collision_configuration = new btDefaultCollisionConfiguration();
	collision_dispatcher = new btCollisionDispatcher(collision_configuration);
	solver = new btSequentialImpulseConstraintSolver();
	dynamics_world = new btDiscreteDynamicsWorld(collision_dispatcher, broadphase, solver, collision_configuration);
	dynamics_world->setGravity(btVector3(0, -10, 0));

	// For the ghost object to work correctly, we need to add a callback to our world.
	dynamics_world->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

	draw_graph.add_node();

	logger::log("[Stage] Done init.");
}

void noob::stage::tear_down() noexcept(true) 
{
	for (size_t i = 0; i < bodies.count(); ++i)
	{
		remove_body(body_handle::make(i));
	}

	bodies.empty();

	// Joints are automatically cleaned up by Bullet once all bodies have been destroyed. I think...
	joints.empty();

	for (size_t i = 0; i < ghosts.count(); ++i)
	{
		remove_ghost(ghost_handle::make(i));
	}

	ghosts.empty();

	actors.empty();
	actor_mq.clear();
	actor_mq_count = 0;

	sceneries.empty();

	delete dynamics_world;
	delete broadphase;
	delete solver;
	delete collision_dispatcher;
	delete collision_configuration;

	draw_graph.empty();

	init();
}


void noob::stage::update(double dt) noexcept(true) 
{
	// PROFILE_FUNC();
	noob::time start_time = noob::clock::now();

	//if (nav_changed)
	//{
	// build_navmesh();
	// nav_changed = false;
	//}

	dynamics_world->stepSimulation(1.0/60.0, 10);

	// update_particle_systems();
	update_actors();
	noob::time end_time = noob::clock::now();

	update_duration = end_time - start_time;

	noob::globals& g = noob::globals::get_instance();
	g.profile_run.stage_physics_duration += update_duration;
}


void noob::stage::draw(float window_width, float window_height, const noob::vec3& eye_pos, const noob::vec3& eye_target, const noob::vec3& eye_up, const noob::mat4& projection_mat) noexcept(true) 
{
	// PROFILE_FUNC();
	const noob::time start_time = noob::clock::now();

	const noob::mat4 view_mat(noob::look_at(eye_pos, eye_target, eye_up));

	bgfx::setViewTransform(0, &view_mat.m[0], &projection_mat.m[0]);
	bgfx::setViewRect(0, 0, 0, window_width, window_height);
	bgfx::setViewTransform(1, &view_mat.m[0], &projection_mat.m[0]);
	bgfx::setViewRect(1, 0, 0, window_width, window_height);

	noob::graphics& gfx = noob::graphics::get_instance();
	bgfx::setUniform(gfx.get_ambient().handle, &ambient_light.v[0]);

	rde::vector<float> instancing_buffer; 
	uint32_t instance_buffer_count = 0;

	noob::globals& g = noob::globals::get_instance();

	const bool doing_instanced = (gfx.instancing_supported() && instancing);










	/*
	   for (uint32_t system_index = 0; system_index < particle_systems.count(); ++system_index)
	   {
	   const noob::particle_system* sys = std::get<1>(particle_systems.get_ptr(noob::particle_system_handle::make(system_index)));
	   if (sys->active)
	   {
	   const noob::scaled_model model = g.model_from_shape(sys->shape);
	   for (uint32_t i = 0; i < noob::particle_system::max_particles; ++i)
	   {
	   const noob::particle p = sys->particles[i];
	   if (p.active)
	   {
	// logger::log("drawing particle!");
	noob::ghost ghst = ghosts.get(p.ghost);

	noob::versor temp_quat = ghst.get_orientation();
	noob::mat4 world_mat = noob::identity_mat4();
	world_mat = noob::rotate(world_mat, temp_quat);
	world_mat = noob::scale(world_mat, model.scales);												
	world_mat = noob::translate(world_mat, ghst.get_position());
	noob::mat4 normal_mat = noob::transpose(noob::inverse((world_mat * view_mat)));

	noob::basic_renderer::uniform u;
	u.colour = p.colour;
	noob::reflectance temp_reflect;

	g.basic_drawer.draw(g.basic_models.get(model.model_h), world_mat, normal_mat, eye_pos, u, g.reflectances.get(sys->reflect), temp_lights, 0);
	}
	}
	}
	}
	*/
	if (show_origin)
	{
		// noob::mat4 world_mat = noob::scale(noob::identity_mat4(), shapes.get(bodies_to_shapes[b.index()])->get_scales());
		// world_mat = bodies.get(b)->get_transform() * world_mat;
		// renderer.draw(basic_models.get(m), shaders.get(s), world_mat);
		// bgfx::setViewTransform(view_id, view_matrix, ortho);
		// bgfx::setViewRect(view_id, 0, 0, window_width, window_height);
		noob::mat4 normal_mat = noob::identity_mat4();

		// renderer.draw(basic_models.get(unit_cube_model), shaders.get(debug_shader), noob::scale(noob::identity_mat4(), noob::vec3(10.0, 10.0, 10.0)), normal_mat, basic_lights);
	}

	// TODO: Benchmark if clear() makes any difference
	// matrix_pool.clear();
	// matrix_pool_count = 0;
	noob::time end_time = noob::clock::now();
	draw_duration = end_time - start_time;
	g.profile_run.stage_draw_duration += draw_duration;
}


void noob::stage::build_navmesh() noexcept(true)
{
	/*	noob::time begin = noob::clock::now();
	// logger::log("[Stage] Clearing out old navmesh data.");
	nav.clear_temporaries();
	nav.clear_geom();

	noob::globals& g = noob::globals::get_instance();
	// logger::log("[Stage] About to add scenery data to navmesh.");

	for (uint32_t i = 0; i < sceneries.count(); ++i)
	{
	noob::scenery sc = sceneries.get(noob::scenery_handle::make(i));
	// logger::log("Got scenery");
	noob::shape_handle shp_h = noob::shape_handle::make(noob::body::get_shape_index(bodies.get(sc.body)));
	// logger::log("Got shape handle");
	noob::basic_mesh m = (g.shapes.get(shp_h)).get_mesh();
	noob::mat4 t = bodies.get(sc.body).get_transform();
	for (uint32_t v = 0; v < m.vertices.size(); ++v)
	{
	noob::vec4 temp = t * noob::vec4(m.vertices[v], 1.0);
	m.vertices[v] = noob::vec3(temp[0], temp[1], temp[2]);
	}
	// fmt::MemoryWriter ww;
	// ww << "Got mesh. v = " << m.vertices.size() << ", i = " << m.indices.size();
	// logger::log(ww.str());
	nav.add_geom(m);
	}

	// logger::log("[Stage] About to build navmesh.");
	nav.build();
	noob::time end = noob::clock::now();

	fmt::MemoryWriter ww;
	ww <<  "[Stage] Navmesh built! Total time: ";
	last_navmesh_build_duration = end - begin;
	ww << noob::pretty_print_timing(last_navmesh_build_duration);
	*/
}


noob::body_handle noob::stage::body(const noob::body_type b_type, const noob::shape_handle shape_h, float mass, const noob::vec3& pos, const noob::versor& orient, bool ccd) noexcept(true) 
{
	noob::globals& g = noob::globals::get_instance();

	noob::body b;
	b.init(dynamics_world, b_type, g.shapes.get(shape_h), mass, pos, orient, ccd);	

	body_handle bod_h = bodies.add(b);
	noob::body* b_ptr = std::get<1>(bodies.get_ptr_mutable(bod_h));

	return bod_h;
}


noob::ghost_handle noob::stage::ghost(const noob::shape_handle shape_h, const noob::vec3& pos, const noob::versor& orient) noexcept(true) 
{
	noob::globals& g = noob::globals::get_instance();

	noob::ghost temp_ghost;
	temp_ghost.init(dynamics_world, g.shapes.get(shape_h), pos, orient);

	noob::ghost_handle ghost_h = ghosts.add(temp_ghost);

	return ghost_h;
}


noob::joint_handle noob::stage::joint(const noob::body_handle a, const noob::vec3& local_point_on_a, const noob::body_handle b, const noob::vec3& local_point_on_b) noexcept(true) 
{
	noob::body bod_a = bodies.get(a);
	noob::body bod_b = bodies.get(b);

	noob::joint j;

	noob::mat4 local_a = noob::translate(noob::identity_mat4(), local_point_on_a);
	noob::mat4 local_b = noob::translate(noob::identity_mat4(), local_point_on_b);

	j.init(dynamics_world, bod_a, bod_b, local_a, local_b);

	noob::joint_handle h = joints.add(j);

	return h;
}


noob::actor_handle noob::stage::actor(const noob::actor_blueprints_handle bp_h, uint32_t team, const noob::vec3& pos, const noob::versor& orient) 
{
	noob::globals& g = noob::globals::get_instance();
	const noob::actor_blueprints bp = g.actor_blueprints.get(bp_h);

	noob::actor a;
	a.ghost = ghost(bp.bounds, pos, orient);
	a.bp_handle = bp_h;

	noob::actor_handle a_h = actors.add(a);

	noob::stage_item_variant var;
	var.type = noob::stage_item_type::ACTOR;
	var.index = a_h.index();
	//noob::handle<noob::stage_item_variant> var_h = stage_item_variants.add(var);

	add_to_graph(bp.shader, bp.bounds, bp.reflect, var);

	noob::ghost temp_ghost = ghosts.get(a.ghost);
	temp_ghost.inner->setUserIndex_1(static_cast<uint32_t>(noob::stage_item_type::ACTOR));
	temp_ghost.inner->setUserIndex_2(a_h.index());

	return a_h;
}


noob::scenery_handle noob::stage::scenery(const noob::shape_handle shape_arg, const noob::shader_variant shader_arg, const noob::reflectance_handle reflect_arg, const noob::vec3& pos_arg, const noob::versor& orient_arg)
{
	noob::globals& g = noob::globals::get_instance();

	const noob::body_handle bod_h = body(noob::body_type::STATIC, shape_arg, 0.0, pos_arg, orient_arg, false);

	noob::scenery sc;
	sc.body = bod_h;
	sc.shader = shader_arg;
	sc.reflect = reflect_arg;
	noob::scenery_handle scenery_h = sceneries.add(sc);


	noob::stage_item_variant var;
	var.type = noob::stage_item_type::SCENERY;
	var.index = bod_h.index();
	// noob::handle<noob::stage_item_variant> var_h = stage_item_variants.add(var);

	add_to_graph(shader_arg, shape_arg, reflect_arg, var);


	noob::body b = bodies.get(bod_h);
	b.inner->setUserIndex_1(static_cast<uint32_t>(noob::stage_item_type::SCENERY));
	b.inner->setUserIndex_2(scenery_h.index());

	return scenery_h;
	// nav_changed = true;
}


void noob::stage::set_light(uint32_t i, const noob::light_handle h) noexcept(true) 
{
	if (i < MAX_LIGHTS)
	{
		lights[i] = h;
	}	
}


void noob::stage::set_directional_light(const noob::directional_light& l) noexcept(true) 
{
	directional_light = l;
}


noob::light_handle noob::stage::get_light(uint32_t i) const noexcept(true) 
{
	noob::light_handle l;

	if (i < MAX_LIGHTS)
	{
		l = lights[i];
	}

	return l;
}


std::vector<noob::contact_point> noob::stage::get_intersecting(const noob::ghost_handle ghost_h) const noexcept(true) 
{
	noob::ghost temp_ghost = ghosts.get(ghost_h);

	btManifoldArray manifold_array;

	btBroadphasePairArray& pairArray = temp_ghost.inner->getOverlappingPairCache()->getOverlappingPairArray();

	std::vector<noob::contact_point> results;

	size_t num_pairs = pairArray.size();

	for (size_t i = 0; i < num_pairs; ++i)
	{
		manifold_array.clear();

		const btBroadphasePair& pair = pairArray[i];

		btBroadphasePair* collision_pair = dynamics_world->getPairCache()->findPair(pair.m_pProxy0, pair.m_pProxy1);

		if (!collision_pair)
		{
			continue;
		}

		if (collision_pair->m_algorithm)
		{
			collision_pair->m_algorithm->getAllContactManifolds(manifold_array);
		}

		for (size_t j = 0; j < manifold_array.size(); ++j)
		{
			btPersistentManifold* manifold = manifold_array[j];
			const btCollisionObject* bt_obj = manifold->getBody0();
			// Sanity check
			const uint32_t index = bt_obj->getUserIndex_2();
			if (index != std::numeric_limits<uint32_t>::max())
			{
				// btScalar direction = is_first_body ? btScalar(-1.0) : btScalar(1.0);
				for (size_t p = 0; p < manifold->getNumContacts(); ++p)
				{
					const btManifoldPoint& pt = manifold->getContactPoint(p);
					if (pt.getDistance() < 0.0f)
					{
						noob::contact_point cp;

						cp.item_type = static_cast<noob::stage_item_type>(bt_obj->getUserIndex_1());
						cp.index = index;
						cp.pos_a = vec3_from_bullet(pt.getPositionWorldOnA());
						cp.pos_b = vec3_from_bullet(pt.getPositionWorldOnB());
						cp.normal_on_b = vec3_from_bullet(pt.m_normalWorldOnB);

						results.push_back(cp);
					}
				}
			}
			else
			{
				logger::log("[Stage] DATA ERROR: Invalid objects found during collision");
			}
		}
	}

	return results;
}


std::vector<noob::contact_point> noob::stage::get_intersecting(const noob::actor_handle ah) const noexcept(true)
{
	noob::actor a = actors.get(ah);
	return get_intersecting(a.ghost);
}


void noob::stage::remove_body(noob::body_handle h) noexcept(true) 
{
	noob::body b = bodies.get(h);
	if (b.physics_valid)
	{
		dynamics_world->removeRigidBody(b.inner);
		delete b.inner;
	}
}


void noob::stage::remove_ghost(noob::ghost_handle h) noexcept(true) 
{
	noob::ghost b = ghosts.get(h);
	dynamics_world->removeCollisionObject(b.inner);
	delete b.inner;
}


void noob::stage::update_actors() noexcept(true)
{
	for (uint32_t i = 0; i < actors.count(); ++i)
	{
		//noob::actor a = actors.get_unsafe(noob::actor_handle::make(i));
		actor_dither(noob::actor_handle::make(i));
	}
}


void noob::stage::actor_dither(noob::actor_handle ah) noexcept(true)
{
	noob::actor a = actors.get(ah);

	if (a.alive)
	{
		std::vector<noob::contact_point> cps = get_intersecting(ah);
		//if (cps.size() == 0)
		//{
		//	noob::vec3 gravity = vec3_from_bullet(dynamics_world->getGravity()) * a.gravity_coeff;
		//	noob::ghost gh = ghosts.get(a.ghost);
		//	noob::vec3 temp_pos = gh.get_position();

		//	temp_pos += gravity;
		//	gh.set_position(temp_pos);
		// }
	}
}


// Start with shader type + index (pack bits). Then do models, and then reflectance. Finally, do stage item type + index (again, pack bits.)
noob::node_handle noob::stage::add_to_graph(const noob::shader_variant shader_arg, const noob::shape_handle shape_arg, const noob::reflectance_handle reflect_arg, const noob::stage_item_variant variant_arg) 
{
	noob::globals& g = noob::globals::get_instance();


	// First, get our root node.
	const noob::node_handle root_node = noob::node_handle::make(0);	


	// Find or create shading node.
	bool shading_found = false;
	noob::node_handle shading_node;
	rde::vector<noob::node_handle> root_children = draw_graph.get_children(root_node);

	for (noob::node_handle n : root_children)
	{
		std::tuple<uint32_t, uint32_t> unpacked = noob::pack_64_to_32(node_masks[n.index()]);
		if (std::get<0>(unpacked) == static_cast<uint32_t>(shader_arg.type) && std::get<1>(unpacked) == static_cast<uint32_t>(shader_arg.handle))
		{
			shading_found = true;
			shading_node = n;
			break;
		}
	}

	if (!shading_found)
	{
		shading_node = draw_graph.add_node();
		node_masks.push_back(noob::pack_32_to_64(static_cast<uint32_t>(shader_arg.type), shader_arg.handle));

		assert(draw_graph.num_nodes() == node_masks.size() && "[Stage] node_masks num must be == draw_graph nodes num");
	}


	// Now onto model node
	
	noob::scaled_model model_scaled = g.model_from_shape(shape_arg);
	
	bool model_found = false;
	noob::node_handle model_node;
	rde::vector<noob::node_handle> shading_children = draw_graph.get_children(shading_node);

	for (noob::node_handle n : shading_children)
	{
		if (node_masks[n.index()] == static_cast<uint64_t>(model_scaled.model_h.index()))
		{
			model_found = true;
			model_node = n;
			break;
		}
	}

	if (!model_found)
	{
		model_node = draw_graph.add_node();
		node_masks.push_back(static_cast<uint64_t>(model_node.index()));

		assert(draw_graph.num_nodes() == node_masks.size() && "[Stage] node_masks num must be == draw_graph nodes num");
	}


	// Reflectance

	bool reflect_found = false;
	noob::node_handle reflect_node;
	rde::vector<noob::node_handle> model_children = draw_graph.get_children(model_node);

	for (noob::node_handle n : model_children)
	{
		if (node_masks[n.index()] == static_cast<uint64_t>(reflect_arg.index()));
		{
			reflect_found = true;
			reflect_node = n;
			break;
		}	
	}

	if (!reflect_found)
	{
		reflect_node = draw_graph.add_node();
		node_masks.push_back(static_cast<uint64_t>(reflect_arg.index()));
	
		assert(draw_graph.num_nodes() == node_masks.size() && "[Stage] node_masks num must be == draw_graph nodes num");
	}

	
	// Now (finally) do the stage item itself :)

	
	bool item_found = false;
	noob::node_handle item_node;
	rde::vector<noob::node_handle> reflect_children = draw_graph.get_children(reflect_node);

	for (noob::node_handle n : reflect_children)
	{
		std::tuple<uint32_t, uint32_t> unpacked = pack_64_to_32(node_masks[n.index()]);

		if (std::get<0>(unpacked) == static_cast<uint32_t>(variant_arg.type) && std::get<1>(unpacked) == static_cast<uint32_t>(variant_arg.index))
		{
			item_found = true;
			item_node = n;
			break;
		}
	}

	if (!item_found)
	{
		item_node = draw_graph.add_node();
		node_masks.push_back(noob::pack_32_to_64(static_cast<uint32_t>(variant_arg.type), variant_arg.index));

		assert(draw_graph.num_nodes() == node_masks.size() && "[Stage] node_masks num must be == draw_graph nodes num");

	}	

	return item_node;

}
/*
   void noob::stage::update_particle_systems() noexcept(true)
   {
// float seconds_since_last = static_cast<float>(noob::divide_duration(update_duration, noob::billion).count());

noob::vec3 world_gravity = noob::vec3_from_bullet(dynamics_world->getGravity());

uint32_t systems_count = particle_systems.count();

for (uint32_t systems_index = 0; systems_index < systems_count; ++systems_index)
{
noob::particle_system* sys = std::get<1>(particle_systems.get_ptr_mutable(noob::particle_system_handle::make(systems_index)));

if (sys->active)
{
// We calculate the new positions of our particles

sys->nanos_accum += update_duration.count();

const float damping = sys->damping;
const float gravity_multiplier = sys->gravity_multiplier;
noob::vec3 wind = sys->wind;

for (uint32_t i = 0; i < noob::particle_system::max_particles; ++i)
{
noob::particle p = sys->particles[i];
if (p.active)
{
p.lifetime += update_duration;

if (p.lifetime.count() < sys->lifespan)
{
// logger::log("updating particle");
std::vector<noob::contact_point> cp = get_intersecting(p.ghost);
// If our particle hasn't gotten into contact with stuff yet...
//if (cp.size() == 0)
//{
// logger::log("updating particle");
p.velocity = (p.velocity + wind + (world_gravity * gravity_multiplier)) * damping;
noob::ghost temp_ghost = ghosts.get(p.ghost);
noob::vec3 pos = temp_ghost.get_position();
pos += p.velocity;;
sys->particles[i] = p;
temp_ghost.set_position(pos);
//}
// Otherwise, deactivate it.
//else
//{
// p.active = false;
// p.lifetime = noob::duration(0);
// sys->first_free = i;
//}
}
else
{
logger::log("kill particle");
p.active = false;
p.lifetime = noob::duration(0);
sys->particles[i] = p;
sys->first_free = i;
}

}

}

// Now, can we possibly spawn a new particle?
particle_spawn_helper(sys);
}
}
}


void noob::stage::particle_spawn_helper(noob::particle_system* sys) noexcept(true)
{
	sys->nanos_accum += update_duration.count();

	if (sys->nanos_accum > sys->nanos_between_emits)
	{
		if (sys->first_free != std::numeric_limits<uint32_t>::max())
		{
			// logger::log("emitting");
			sys->nanos_accum = 0;

			noob::globals& g = noob::globals::get_instance();

			noob::particle p;
			p.active = true;
			p.lifetime = noob::duration(0);

			noob::vec3 temp = sys->emit_direction_variance;
			//noob::vec3 spread = noob::vec3(g.get_random() * temp[0], g.get_random() * temp[1], g.get_random() * temp[2]);
			noob::vec3 spread = noob::vec3(1.0, 1.0, 1.0);
			noob::vec3 dir = sys->emit_direction;

			p.velocity = noob::vec3(dir[0] + spread[0], dir[1] + spread[1], dir[2] + spread[2]) * sys->emit_force;

			noob::ghost ghst = ghosts.get(p.ghost);
			ghst.set_position(sys->center);


			sys->particles[sys->first_free] = p;

			uint32_t next = sys->first_free;
			next = sys->get_next_free(next);
			sys->first_free = next;
		}
	}
}
*/


/*
   void noob::stage::write_graph(const std::string& filename) const
   {
   logger::log("About to write graph");
   boost::filesystem::path p("temp/");
   if (boost::filesystem::exists(p))
   {
   std::string full_path_str = p.generic_string() + filename;
   lemon::digraphWriter(draw_graph, full_path_str).nodeMap("model", basic_models_mapping).nodeMap("body", bodies_mapping).nodeMap("shader", shaders_mapping).node("root", root_node).run();
   }
   else logger::log("[Stage] Could not write graph snapshot - temp directory not found.");
   }
   */

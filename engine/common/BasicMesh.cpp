// TODO: Remove the following comment if it works across all builds
//#define CSGJS_HEADER_ONLY
//#include <csgjs.hpp>
//


#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <string>
#include <iostream>

#include "Logger.hpp"
#include "BasicMesh.hpp"
#include "format.h"


#include <igl/read_triangle_mesh.h>
#include <igl/write_triangle_mesh.h>


noob::vec3 noob::basic_mesh::get_vertex(unsigned int i)
{
	if (i > V.size())
	{
		logger::log("[BasicMesh] - Attempting to get invalid vertex");
		return noob::vec3(V.col(0));
	}
	else
	{
		return noob::vec3(V.col(i));
	}
}


noob::vec3 noob::basic_mesh::get_normal(unsigned int i)
{
	if (i > N.size())
	{
		logger::log("[BasicMesh] - Attempting to get invalid normal");
		//Eigen::Vector3d pp = N[0];
		return noob::vec3(N.col(i));
	}
	else
	{
		return noob::vec3(N.col(i));
	}
}

/*
noob::vec3 noob::basic_mesh::get_texcoord(unsigned int i)
{
	if (i > texcoords.size())
	{
		logger::log("[BasicMesh] - Attempting to get invalid texcoord");
		return texcoords[0];
	}
	else return texcoords[i];
}


unsigned int noob::basic_mesh::get_index(unsigned int i)
{
	if (i > indices.size())
	{
		logger::log("[BasicMesh] - Attempting to get invalid index");
		return indices[0];
	}
	else return indices[i];

}


void noob::basic_mesh::set_vertex(unsigned int i, const noob::vec3& v)
{
	if (i > vertices.size())
	{
		logger::log("[BasicMesh] - Attempting to set invalid vertex");
	}
	else vertices[i] = v;
}


void noob::basic_mesh::set_normal(unsigned int i, const noob::vec3& v)
{
	if (i > normals.size())
	{
		logger::log("[BasicMesh] - Attempting to set invalid normal");
	}
	else normals[i] = v;
}


void noob::basic_mesh::set_texcoord(unsigned int i, const noob::vec3& v)
{
	if (i > texcoords.size())
	{
		logger::log("[BasicMesh] - Attempting to set invalid texcoord");
	}
	else texcoords[i] = v;
}


void noob::basic_mesh::set_index(unsigned int i, unsigned int v)
{
	if (i > indices.size())
	{
		logger::log("[BasicMesh] - Attempting to set invalid index");
	}
	else indices[i] = static_cast<uint32_t>(v);

}
*/

double noob::basic_mesh::get_volume()
{
	if (!volume_calculated)
	{
		// Proudly gleaned from U of R website!
		// http://mathcentral.uregina.ca/QQ/database/QQ.09.09/h/ozen1.html
		// Woot alma mater! (... Soon :P)
		// The volume of the tetrahedron with vertices (0 ,0 ,0), (a1 ,a2 ,a3), (b1, b2, b3) and (c1, c2, c3) is [a1b2c3 + a2b3c1 + a3b1c2 - a1b3c2 - a2b1c3 - a3b2c1] / 6.
		double accum = 0.0;
		for (size_t i = 0; i < F.size(); ++i)
		{
			Eigen::Vector3i ff = F.col(i);
			noob::vec3 first(V.col(ff[0]));
			noob::vec3 second(V.col(ff[1])); // = vertices[i+1];
			noob::vec3 third(V.col(ff[2])); // = vertices[i+2];

			accum += ((static_cast<double>(first.v[0]) * static_cast<double>(second.v[1]) * static_cast<double>(third.v[2])) + (static_cast<double>(first.v[1]) * static_cast<double>(second.v[2]) * static_cast<double>(third.v[0])) + (static_cast<double>(first.v[2]) * static_cast<double>(second.v[0]) * static_cast<double>(third.v[1])) - (static_cast<double>(first.v[0]) * static_cast<double>(second.v[2]) * static_cast<double>(third.v[1])) - (static_cast<double>(first.v[1]) * static_cast<double>(second.v[0]) * static_cast<double>(third.v[2])) - (static_cast<double>(first.v[2]) * static_cast<double>(second.v[1]) * static_cast<double>(third.v[0]))) / 6.0;

		}
		volume_calculated = true;
		volume = accum;
	}

	return volume;
}


noob::basic_mesh noob::basic_mesh::decimate(size_t num_verts) const
{
	//logger::log("[Mesh] decimating");
	TriMesh half_edges = to_half_edges();
	// logger::log(fmt::format("[Mesh] decimating - Half edges generated. num verts = {0}", half_edges.n_vertices()));

	OpenMesh::Decimater::DecimaterT<TriMesh> decimator(half_edges);
	OpenMesh::Decimater::ModQuadricT<TriMesh>::Handle mod;
	decimator.add(mod);
	decimator.initialize();

	auto did_init = decimator.initialize();
	if (!did_init) 
	{
		// logger::log("[Mesh] decimate() - init failed!");
		return noob::basic_mesh(*this);
	}

	size_t verts_removed = decimator.decimate_to(num_verts);
	logger::log(fmt::format("[Mesh] decimate() - Vertices removed = {0}", verts_removed));
	decimator.mesh().garbage_collection();
	half_edges.garbage_collection();
	noob::basic_mesh temp;
	temp.from_half_edges(half_edges);
	return temp;
}


void noob::basic_mesh::normalize() 
{
	// std::string temp = save();
	// load_mem(temp);
}

/*
// TODO
void noob::basic_mesh::to_origin()
{
	// noob::basic_mesh temp;
	for (size_t i = 0; i < vertices.size(); ++i)
	{
		vertices[i] = (vertices[i] - bbox_info.center);
	}
	normalize();

}
*/
/*
std::string noob::basic_mesh::save() const
{
	fmt::MemoryWriter w;
	w << "OFF" << "\n" << vertices.size() << " " << indices.size() / 3 << " " << 0 <<  "\n";
	for (auto v : vertices)
	{
		w << v.v[0] << " " << v.v[1] << " " << v.v[2] <<  "\n";
	}
	for (size_t i = 0; i < indices.size(); i = i + 3)
	{
		w << 3 << " " << indices[i] << " " << indices[i+1] << " " << indices[i+2] << "\n";
	}

	return w.str();
}
*/

bool noob::basic_mesh::save(const std::string& filename) const
{
	return igl::write_triangle_mesh(filename, V, F);
}

/*
bool noob::basic_mesh::load_mem(const std::string& file, const std::string& name)
{
	const aiScene* scene = aiImportFileFromMemory(file.c_str(), file.size(), post_process, "");//aiProcessPreset_TargetRealtime_Fast | aiProcess_FixInfacingNormals, "");
	return load_assimp(scene, name);
}
*/

bool noob::basic_mesh::load_file(const std::string& filename, const std::string& name)
{

	return igl::read_triangle_mesh(filename, V, F);
}


/*
void noob::basic_mesh::transform(const noob::mat4& transform)
{

	for (size_t i = 0; i < vertices.size(); i++)
	{
		noob::vec3 v = vertices[i];
		noob::vec4 temp_vert(v, 1.0);
		noob::vec4 temp_transform = transform * temp_vert;
		noob::vec3 transformed_vert;

		transformed_vert[0] = temp_transform[0];
		transformed_vert[1] = temp_transform[1];
		transformed_vert[2] = temp_transform[2];

		vertices[i] = temp_transform;
	}

	normalize();
}


void noob::basic_mesh::translate(const noob::vec3& translation)
{
	noob::transform_helper t;
	t.translate(translation);
	transform(t.get_matrix());
}


void noob::basic_mesh::rotate(const noob::versor& orientation)
{
	noob::transform_helper t;
	t.rotate(orientation);
	transform(t.get_matrix());
}


void noob::basic_mesh::scale(const noob::vec3& scale)
{
	noob::transform_helper t;
	t.scale(scale);
	transform(t.get_matrix());

}
*/

TriMesh noob::basic_mesh::to_half_edges() const
{
	TriMesh half_edges;
	std::vector<TriMesh::VertexHandle> vert_handles;

	for (size_t i = 0; i < V.size(); ++i)
	{
		Eigen::Vector3d pp = V.col(i);
		vert_handles.push_back(half_edges.add_vertex(TriMesh::Point(static_cast<float>(pp[0]), static_cast<float>(pp[1]), static_cast<float>(pp[2]))));
	}

	for (size_t i = 0; i < F.size(); ++i)
	{
		std::vector<TriMesh::VertexHandle> face_verts;
		Eigen::Vector3i ff = F.col(i);

		face_verts.push_back(vert_handles[ff[0]]);
		face_verts.push_back(vert_handles[ff[1]]);
		face_verts.push_back(vert_handles[ff[2]]);
		
		half_edges.add_face(face_verts);
	}

	return half_edges;
}


void noob::basic_mesh::from_half_edges(TriMesh half_edges)
{
	V.resize(Eigen::NoChange, half_edges.n_vertices());
	for (TriMesh::ConstVertexIter v_it = half_edges.vertices_begin(); v_it != half_edges.vertices_end(); ++v_it)
	{
		TriMesh::Point p = half_edges.point(*v_it);
		size_t col_index = v_it->idx();
		V.setConstant(col_index, 0, p[0]);
		V.setConstant(col_index, 1, p[1]);
		V.setConstant(col_index, 2, p[2]);
	}

	F.resize(Eigen::NoChange, half_edges.n_faces());
	for (TriMesh::ConstFaceIter f_it = half_edges.faces_begin(); f_it != half_edges.faces_end(); ++f_it)
	{
		size_t tri_vert_index = 0;
		for (TriMesh::FaceVertexCCWIter fv_it = half_edges.fv_ccwiter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			F.setConstant(f_it->idx(), tri_vert_index, fv_it->idx());
			++tri_vert_index;
		}
	}
}


void noob::basic_mesh::from_half_edges(PolyMesh _half_edges)
{
	// std::ostringstream oss;
	PolyMesh half_edges = _half_edges;
	half_edges.triangulate();
	half_edges.garbage_collection();
	
	V.resize(Eigen::NoChange, half_edges.n_vertices());
	for (PolyMesh::ConstVertexIter v_it = half_edges.vertices_begin(); v_it != half_edges.vertices_end(); ++v_it)
	{
		PolyMesh::Point p = half_edges.point(*v_it);
		size_t col_index = v_it->idx();
		V.setConstant(col_index, 0, p[0]);
		V.setConstant(col_index, 1, p[1]);
		V.setConstant(col_index, 2, p[2]);
	}

	F.resize(Eigen::NoChange, half_edges.n_faces());
	for (PolyMesh::ConstFaceIter f_it = half_edges.faces_begin(); f_it != half_edges.faces_end(); ++f_it)
	{
		size_t tri_vert_index = 0;
		for (PolyMesh::FaceVertexCCWIter fv_it = half_edges.fv_ccwiter(*f_it); fv_it.is_valid(); ++fv_it)
		{
			F.setConstant(f_it->idx(), tri_vert_index, fv_it->idx());
			++tri_vert_index;
		}
	}
	//from_half_edges(reinterpret_cast<TriMesh>(_half_edges));
}

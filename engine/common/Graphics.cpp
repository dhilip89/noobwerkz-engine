#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Graphics.hpp"

#include <algorithm>

#include <GLES3/gl3.h>

#include "ShadersGL.hpp"
#include "NoobUtils.hpp"
#include "StringFuncs.hpp"


GLenum check_error_gl(const char *file, int line)
{
	GLenum error_code;
	while ((error_code = glGetError()) != GL_NO_ERROR)
	{
		std::string error;
		switch (error_code)
		{
			case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
			case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
			case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
							       // TODO: Find out if these are still part of the modern GL
							       // case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
							       // case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
			case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
			case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
		}
		noob::logger::log(noob::importance::ERROR, noob::concat("OpenGL: ", error, file, " (", noob::to_string(line), ")"));
	}
	return error_code;
}

#define check_error_gl() check_error_gl(__FILE__, __LINE__) 


GLuint load_shader_gl(GLenum type, const std::string& shader_arg)
{
	GLuint shader;
	GLint compiled;

	// Create the shader object
	shader = glCreateShader(type);

	if (shader == 0)
	{
		return 0;
	}

	const char* shader_src = shader_arg.c_str();
	// Load the shader source
	glShaderSource(shader, 1, &shader_src, NULL);

	// Compile the shader
	glCompileShader(shader);

	// Check the compile status
	glGetShaderiv (shader, GL_COMPILE_STATUS, &compiled);

	if (!compiled)
	{
		GLint info_len = 0;

		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &info_len);

		if (info_len > 1)
		{
			std::string info_log;
			info_log.resize(info_len);

			glGetShaderInfoLog(shader, info_len, NULL, &info_log[0]);

			noob::logger::log(noob::importance::ERROR, noob::concat("[Graphics] Error compiling shader: ", info_log));
		}

		glDeleteShader(shader);
		return 0;
	}

	return shader;
}

GLuint load_program_gl(const std::string& vert_shader_arg, const std::string frag_shader_arg)
{
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint program_object;
	GLint linked;

	// Just to make sure no dirty state gets to ruin our shader compile. :)
	glBindVertexArray(0);

	const char* vert_shader_src = vert_shader_arg.c_str();
	// Load the vertex/fragment shaders
	vertex_shader = load_shader_gl(GL_VERTEX_SHADER, vert_shader_arg);

	if (vertex_shader == 0)
	{
		return 0;
	}

	const char* frag_shader_src  = frag_shader_arg.c_str();
	fragment_shader = load_shader_gl(GL_FRAGMENT_SHADER, frag_shader_arg);

	if (fragment_shader == 0)
	{
		glDeleteShader(vertex_shader);
		return 0;
	}

	// Create the program object
	program_object = glCreateProgram();

	if (program_object == 0)
	{
		return 0;
	}

	glAttachShader(program_object, vertex_shader);
	glAttachShader(program_object, fragment_shader);

	// Link the program
	glLinkProgram(program_object);

	// Check the link status
	glGetProgramiv(program_object, GL_LINK_STATUS, &linked);

	if (!linked)
	{
		GLint info_len = 0;

		glGetProgramiv(program_object, GL_INFO_LOG_LENGTH, &info_len);

		if (info_len > 1)
		{
			std::string info_log;
			info_log.resize(info_len);

			glGetProgramInfoLog(program_object, info_len, NULL, &info_log[0]);
			noob::logger::log(noob::importance::ERROR, noob::concat("[Graphics] Error linking program:", info_log));

		}

		glDeleteProgram(program_object);
		return 0;
	}

	// Free up no longer needed shader resources
	glDeleteShader(vertex_shader);
	glDeleteShader(fragment_shader);

	return program_object;
}

GLenum tex_internal_format(const noob::texture_info TexInfo)
{
	/*	struct texture_info
		{
		noob::texture_channels channels;
		std::array<noob::scalar_type, 4> channel_scalars;
		std::array<uint8_t, 4> channel_bits;
		bool compressed, mips, s_normalized;
		}; */



	if (!TexInfo.compressed)
	{
		switch (TexInfo.channels)
		{
			case (noob::texture_channels::R):
				{
					/*		GL_R8,
							GL_R8_SNORM,
							GL_R16F,
							GL_R32F,
							GL_R8UI,
							GL_R8I,
							GL_R16UI,
							GL_R16I,
							GL_R32UI,
							GL_R32I, */
				}
			case (noob::texture_channels::RG):
				{
					/*	GL_RG8,
						GL_RG8_SNORM,
						GL_RG16F,
						GL_RG32F,
						GL_RG8UI,
						GL_RG8I,
						GL_RG16UI,
						GL_RG16I,
						GL_RG32UI,
						GL_RG32I, */

				}
			case (noob::texture_channels::RGB):
				{
					/*	GL_RGB8,
						GL_SRGB8,
						GL_RGB565,
						GL_RGB8_SNORM,
						GL_R11F_G11F_B10F,
						GL_RGB9_E5,
						GL_RGB16F,
						GL_RGB32F,
						GL_RGB8UI,
						GL_RGB8I,
						GL_RGB16UI,
						GL_RGB16I,
						GL_RGB32UI,
						GL_RGB32I, */

				}
			case (noob::texture_channels::RGBA):
				{
					/*	GL_RGBA8,
						GL_SRGB8_ALPHA8,
						GL_RGBA8_SNORM,
						GL_RGB5_A1,
						GL_RGBA4,
						GL_RGB10_A2,
						GL_RGBA16F,
						GL_RGBA32F,
						GL_RGBA8UI,
						GL_RGBA8I,
						GL_RGB10_A2UI,
						GL_RGBA16UI,
						GL_RGBA16I,
						GL_RGBA32I,
						GL_RGBA32UI, */
				}

		};

	}
	else // We are using compression
	{
		/*
		   GL_COMPRESSED_R11_EAC,
		   GL_COMPRESSED_SIGNED_R11_EAC,
		   GL_COMPRESSED_RG11_EAC,
		   GL_COMPRESSED_SIGNED_RG11_EAC,
		   GL_COMPRESSED_RGB8_ETC2,
		   GL_COMPRESSED_SRGB8_ETC2,
		   GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2,
		   GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2,
		   GL_COMPRESSED_RGBA8_ETC2_EAC,
		   GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC
		   */
	}

	return 0;
}

/*
   GL_DEPTH_COMPONENT16 	GL_DEPTH_COMPONENT 	GL_UNSIGNED_SHORT, GL_UNSIGNED_INT 	16 	 
   GL_DEPTH_COMPONENT24 	GL_DEPTH_COMPONENT 	GL_UNSIGNED_INT 			24 	 
   GL_DEPTH_COMPONENT32F 	GL_DEPTH_COMPONENT 	GL_FLOAT 				f32 	 
   GL_DEPTH24_STENCIL8 	GL_DEPTH_STENCIL 	GL_UNSIGNED_INT_24_8 			24 	8
   GL_DEPTH32F_STENCIL8	GL_DEPTH_STENCIL	GL_FLOAT_32_UNSIGNED_INT_24_8_REV 	f32 	8
   */
void noob::graphics::init(uint32_t width, uint32_t height) noexcept(true)
{

	instanced_shader = noob::graphics::program_handle::make(load_program_gl(noob::glsl::vs_instancing_src, noob::glsl::fs_instancing_src));

	u_eye_pos = glGetUniformLocation(instanced_shader.index(), "eye_pos");

	check_error_gl();

	u_light_directional = glGetUniformLocation(instanced_shader.index(), "directional_light");

	check_error_gl();

	frame(width, height);

	noob::logger::log(noob::importance::INFO, "[Graphics] Done init.");


}

void noob::graphics::destroy() noexcept(true)
{

}
/*
   noob::model_handle noob::graphics::model(noob::model::geom_type geom, const noob::basic_mesh& mesh) noexcept(true)
   {
   noob::model_handle results;
   GLuint vao_id = 0;

   switch (geom)
   {
   case(noob::model::geom_type::INDEXED_MESH):
   {
   break;
   }
   case(noob::model::geom_type::DYNAMIC_TERRAIN):
   {
   break;
   }
   case(noob::model::geom_type::BILLBOARD):
   {
   break;
   }
   case(noob::model::geom_type::POINT_SPRITE):
   {
   break;
   }
   default:
   {
   noob::logger::log(noob::importance::ERROR, "[Graphics] Reached past the valid enum values in switch statement. WTF?!");
   }
   }

// Reset to the default VAO
glBindVertexArray(0);

return noob::model_handle::make(vao_id);
}
*/


void noob::graphics::use_program(noob::graphics::program_handle arg) noexcept(true)
{
	glUseProgram(arg.index());
}

noob::model_handle noob::graphics::model_instanced(const noob::basic_mesh& mesh, uint32_t num_instances) noexcept(true)
{
	noob::model result;

	if (num_instances == 0)
	{
		noob::model_handle h;
		return h;
	}

	result.type = noob::model::geom_type::INDEXED_MESH;

	result.n_instances = num_instances;

	const uint32_t num_indices = mesh.indices.size();
	result.n_indices = num_indices;

	const uint32_t num_verts = mesh.vertices.size();
	result.n_vertices = num_verts;

	GLuint vao_id = 0;

	glGenVertexArrays(1, &vao_id);

	glBindVertexArray(vao_id);
	result.vao = vao_id;

	////////////////////////////////
	// Create & bind attrib buffers
	////////////////////////////////

	std::array<GLuint, 4> vbo_ids;
	glGenBuffers(4, &vbo_ids[0]);

	const GLuint vertices_vbo = vbo_ids[0];
	const GLuint colours_vbo = vbo_ids[1];
	const GLuint matrices_vbo = vbo_ids[2];
	const GLuint indices_vbo = vbo_ids[3];

	/////////////////////////////////////////////////
	// Setup non-instanced, interleaved attribs VBO:
	/////////////////////////////////////////////////

	std::vector<noob::vec4> interleaved;
	interleaved.resize(num_verts * 3);

	// Interleave our vertex positions, normals, and colours
	for(uint32_t i = 0; i < num_verts; i++)
	{
		const uint32_t current_offset = i * 3;
		interleaved[current_offset] = noob::vec4(mesh.vertices[i].v[0], mesh.vertices[i].v[1], mesh.vertices[i].v[2], 1.0);
		interleaved[current_offset + 1] = noob::vec4(mesh.normals[i].v[0], mesh.normals[i].v[1], mesh.normals[i].v[2], 0.0);
		interleaved[current_offset + 2] = mesh.colours[i];
	}

	// Upload interleaved buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertices_vbo);
	glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(noob::vec4), &interleaved[0], GL_STATIC_DRAW);
	result.vertices_vbo = vertices_vbo;


	//////////////////////
	// Setup attrib specs
	//////////////////////

	// Positions
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4)*3, reinterpret_cast<const void *>(0));
	// Normals
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4)*3, reinterpret_cast<const void *>(sizeof(noob::vec4)));
	// Vertex colours
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4)*3, reinterpret_cast<const void *>(sizeof(noob::vec4)*2));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	// Setup colours VBO:
	// std::vector<noob::vec4> colours(num_instances, noob::vec4(1.0, 1.0, 1.0, 1.0));
	glBindBuffer(GL_ARRAY_BUFFER, colours_vbo);
	glBufferData(GL_ARRAY_BUFFER, num_instances * noob::model::materials_stride, nullptr, GL_DYNAMIC_DRAW);
	//glBufferData(GL_ARRAY_BUFFER, num_instances * noob::model::materials_stride, &colours[0].v[0], GL_DYNAMIC_DRAW);

	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(noob::vec4), reinterpret_cast<const void *>(0));
	glEnableVertexAttribArray(3);
	glVertexAttribDivisor(3, 1);
	result.instanced_colour_vbo = colours_vbo;

	// Setup matrices VBO:
	// std::vector<noob::mat4> matrices(num_instances * 2, noob::identity_mat4());
	glBindBuffer(GL_ARRAY_BUFFER, matrices_vbo);
	glBufferData(GL_ARRAY_BUFFER, num_instances * noob::model::matrices_stride, nullptr, GL_DYNAMIC_DRAW);
	// glBufferData(GL_ARRAY_BUFFER, num_instances * noob::model::matrices_stride, &matrices[0].m[0], GL_DYNAMIC_DRAW);


	// Per instance model matrices
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(0));
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)));
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)*2));
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)*3));
	// Per-instance MVP matrices
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)*4));
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)*5));
	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)*6));
	glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(noob::mat4)*2, reinterpret_cast<const void *>(sizeof(noob::vec4)*7));

	glEnableVertexAttribArray(4);
	glEnableVertexAttribArray(5);
	glEnableVertexAttribArray(6);
	glEnableVertexAttribArray(7);
	glEnableVertexAttribArray(8);
	glEnableVertexAttribArray(9);
	glEnableVertexAttribArray(10);
	glEnableVertexAttribArray(11);

	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
	glVertexAttribDivisor(9, 1);
	glVertexAttribDivisor(10, 1);
	glVertexAttribDivisor(11, 1);

	result.instanced_matrices_vbo = matrices_vbo;

	// Upload to indices buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices_vbo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(uint32_t), &mesh.indices[0], GL_STATIC_DRAW);
	result.indices_vbo = indices_vbo;


	glBindVertexArray(0);

	check_error_gl();

	noob::model_handle h = models.add(result);

	const noob::bbox bb = mesh.bbox;
	noob::logger::log(noob::importance::INFO, noob::concat("[Graphics] Created model with handle ", noob::to_string(h.index()), " and ", noob::to_string(num_instances), " instances. Verts = ", noob::to_string(mesh.vertices.size()), ", indices = ", noob::to_string(mesh.indices.size()), ". Dims: ", noob::to_string(bb.max - bb.min)));

	return h;
}


void noob::graphics::reset_instances(noob::model_handle h, uint32_t num_instances) noexcept(true)
{
	noob::model m = models.get(h);
	m.n_instances = num_instances;
	models.set(h, m);

	glBindVertexArray(m.vao);

	// Setup colours VBO:
	glBindBuffer(GL_ARRAY_BUFFER, m.instanced_colour_vbo);
	glBufferData(GL_ARRAY_BUFFER, num_instances * noob::model::materials_stride, nullptr, GL_DYNAMIC_DRAW);

	// Setup matrices VBO:
	glBindBuffer(GL_ARRAY_BUFFER, m.instanced_matrices_vbo);
	glBufferData(GL_ARRAY_BUFFER, num_instances * noob::model::matrices_stride, nullptr, GL_DYNAMIC_DRAW);

	check_error_gl();

	glBindVertexArray(0);
}

noob::texture_2d_handle noob::graphics::reserve_texture_2d(uint32_t width, uint32_t height, const noob::texture_info) noexcept(true)
{
	// Prevent leftover from previous calls from harming this.
	glBindVertexArray(0);

	GLuint texture_id;

	glGenTextures(1, &texture_id);

	glBindTexture(GL_TEXTURE_2D, texture_id);


	noob::texture_2d_handle t;
	return t;
}


noob::texture_array_2d_handle noob::graphics::reserve_array_texture_2d(uint32_t width, uint32_t height, uint32_t indices, const noob::texture_info) noexcept(true)
{
	noob::texture_array_2d_handle t;
	return t;
}


noob::texture_3d_handle noob::graphics::reserve_texture_3d(uint32_t width, uint32_t height, uint32_t depth, const noob::texture_info) noexcept(true)
{
	noob::texture_3d_handle t;
	return t;
}



void noob::graphics::texture_data(noob::texture_2d_handle, const std::string&) const noexcept(true)
{

}


void noob::graphics::texture_data(noob::texture_array_2d_handle, uint32_t index, const std::string&) const noexcept(true)
{

}


void noob::graphics::texture_data(noob::texture_3d_handle, const std::string&) const noexcept(true)
{

}


// Texture parameter setters, made typesafe. :)
void noob::graphics::texture_base_level(uint32_t arg) const noexcept(true)
{

}


void noob::graphics::texture_compare_mode(noob::tex_compare_mode) const noexcept(true)
{

}


void noob::graphics::texture_compare_func(noob::tex_compare_func) const noexcept(true)
{

}


void noob::graphics::texture_min_filter(noob::tex_min_filter) const noexcept(true)
{

}


void noob::graphics::texture_min_lod(int32_t arg) const noexcept(true)
{

}


void noob::graphics::texture_max_lod(int32_t arg) const noexcept(true)
{

}


void noob::graphics::texture_swizzle(const std::array<noob::tex_swizzle, 4 >) const noexcept(true)
{

}


void noob::graphics::texture_wrap_mode(const std::array<noob::tex_wrap_mode, 3>) const noexcept(true)
{

}


void noob::graphics::texture_pack_alignment(uint32_t) const noexcept(true)
{
	// GL_PACK_ALIGNMENT 	integer 	4 	1, 2, 4, or 8

}


void noob::graphics::texture_unpack_alignment(uint32_t) const noexcept(true)
{
	// GL_UNPACK_ALIGNMENT 	integer 	4 	1, 2, 4, or 8 

}



void noob::graphics::draw(const noob::model_handle handle, uint32_t num) const noexcept(true)
{
	const noob::model m = models.get(handle);
	glBindVertexArray(m.vao);

	glDrawElementsInstanced(GL_TRIANGLES, m.n_indices, GL_UNSIGNED_INT, reinterpret_cast<const void *>(0), m.n_instances); //std::min(m.n_instances, num));

	check_error_gl();
	glBindVertexArray(0);
}


void noob::graphics::frame(uint32_t width, uint32_t height) const noexcept(true)
{
	glBindVertexArray(0);

	glViewport(0, 0, width, height);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glClearColor(0.2f, 0.0f, 0.2f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}


noob::gpu_write_buffer noob::graphics::map_buffer(noob::model_handle h, noob::model::instanced_data_type t, uint32_t min, uint32_t max) const noexcept(true)
{
	noob::model m = models.get(h);

	if (m.type != noob::model::geom_type::INDEXED_MESH)
	{
		return noob::gpu_write_buffer::make_invalid();
	}

	// glBindVertexArray(m.vao);	

	uint32_t stride_in_bytes;

	switch (t)
	{
		case (noob::model::instanced_data_type::COLOUR):
			{
				stride_in_bytes = noob::model::materials_stride;
				glBindBuffer(GL_ARRAY_BUFFER, m.instanced_colour_vbo);
				check_error_gl();
				break;
			}
		case (noob::model::instanced_data_type::MATRICES):
			{
				stride_in_bytes = noob::model::matrices_stride;
				glBindBuffer(GL_ARRAY_BUFFER, m.instanced_matrices_vbo);
				check_error_gl();
				break;
			}
	}

	const uint32_t total_size = stride_in_bytes * m.n_instances;

	float* ptr = reinterpret_cast<float*>(glMapBufferRange(GL_ARRAY_BUFFER, min, max, GL_MAP_WRITE_BIT));

	check_error_gl();

	if (ptr != nullptr)
	{
		return noob::gpu_write_buffer(ptr, total_size / sizeof(float));
	}
	else
	{
		return noob::gpu_write_buffer::make_invalid();	
	}


}


void noob::graphics::unmap_buffer() const noexcept(true)
{
	glUnmapBuffer(GL_ARRAY_BUFFER);

	check_error_gl();
}


noob::graphics::program_handle noob::graphics::get_instanced() const noexcept(true)
{
	return instanced_shader;
}


void noob::graphics::eye_pos(const noob::vec3& arg) const noexcept(true)
{
	glUniform3fv(u_eye_pos, 1, &arg.v[0]);

	check_error_gl();
}


void noob::graphics::light_direction(const noob::vec3& arg) const noexcept(true)
{
	glUniform3fv(u_light_directional, 1, &arg.v[0]);

	check_error_gl();
}

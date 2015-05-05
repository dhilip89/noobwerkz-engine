#pragma once

#include <map>

#include <bgfx.h>
#include "NoobUtils.hpp"

namespace noob
{
	class graphics
	{
		public:
			struct pos_norm_uv_bones_vertex
			{
				float m_x;
				float m_y;
				float m_z;
				//float m_tangent_x;
				//float m_tangent_y;
				//float m_tangent_z;
				//float m_bitangent_x;
				//float m_bitangent_y;
				//float m_bitangent_z;
				float m_normal_x;
				float m_normal_y;
				float m_normal_z;
				float m_u;
				float m_v;
				//uint8_t m_boneindex[4];
				//float m_boneweight[4];

				static void init()
				{
					ms_decl
						.begin()
						.add(bgfx::Attrib::Position,  3, bgfx::AttribType::Float)
						//.add(bgfx::Attrib::Tangent,   3, bgfx::AttribType::Float)
						//.add(bgfx::Attrib::Bitangent, 3, bgfx::AttribType::Float)
						.add(bgfx::Attrib::Normal,    3, bgfx::AttribType::Float)
						.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
						//.add(bgfx::Attrib::Indices,   4, bgfx::AttribType::Uint8, false, true)
						//.add(bgfx::Attrib::Weight,    4, bgfx::AttribType::Float)
						.end();
				}
				static bgfx::VertexDecl ms_decl;
			};


			static std::map<std::string, bgfx::TextureInfo*> global_textures;
			static std::map<std::string, bgfx::ProgramHandle> programs;

			static void init(uint32_t width, uint32_t height)
			{
				uint32_t reset = BGFX_RESET_VSYNC;

				bgfx::reset(width, height, reset);

				bgfx::setViewClear(0
						, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
						, 0x703070ff
						, 1.0f
						, 0
						);
			}

			static const bgfx::Memory* get_bgfx_mem(const std::string& handle)
			{
				auto it = noob::utils::data.find(handle);
				
				std::string payload;

				if (it != noob::utils::data.end())
				{
					payload = it->second;
				}

				bgfx::Memory* mem = new bgfx::Memory();

				mem->data = (uint8_t*) &payload[0];
				mem->size = payload.size();

				return const_cast<const bgfx::Memory*>(mem);
			
			}


			static void draw(uint32_t width, uint32_t height)
			{
				// Set view 0 default viewport.
				bgfx::setViewRect(0, 0, 0, width, height);

				// This dummy draw call is here to make sure that view 0 is cleared
				// if no other draw calls are submitted to view 0.
				bgfx::submit(0);

				// Advance to next frame. Rendering thread will be kicked to
				// process submitted rendering primitives.
				bgfx::frame();
			}

		/*
		 	static bgfx::TextureInfo* load_texture(const std::string& _name, uint32_t _flags, uint8_t _skip)
			{
				std::string file_path = "textures/";

				file_path.append(_name);
				const bgfx::Memory* mem = noob::utils::load_to_memory_bgfx(file_path);
				bgfx::TextureInfo* _info = new bgfx::TextureInfo();
				bgfx::createTexture(mem, _flags, _skip, _info);
				return _info;
			}
*/


			/*
			   static bgfx::TextureHandle load_texture(const std::string& _path)
			   {
			   bgfx::Memory mem;

			   std::tuple<const uint8_t*,size_t> data = noob::utils::load_to_memory(_path);
			   mem.data = const_cast<uint8_t*>(std::get<0>(data));
			   mem.size = std::get<1>(data);

			   const auto tex_width = 128;
			   const auto tex_height = 128;
			   uint8_t tex_num_mips = 7;
			   return bgfx::createTexture2D(tex_width, tex_height, tex_num_mips, bgfx::TextureFormat::Enum::RGBA8, BGFX_TEXTURE_NONE, &mem); 

			//	bgfx::createTexture(mem, _flags, _skip, _info);

			// bgfx::TextureInfo tex;
			// bgfx::

			// tex.handle = 
			// global_textures->insert(std::make_pair<std::string, bgfx::TextureInfo>(_path, ));
			}
			*/
			static bgfx::ShaderHandle load_shader(const std::string& filename)
			{
				logger::log("Loading shader");
				std::string shader_path = "shaders/dx9/";

				switch (bgfx::getRendererType() )
				{
					case bgfx::RendererType::Direct3D11:
					case bgfx::RendererType::Direct3D12:
						shader_path = "shaders/dx11/";
						break;

					case bgfx::RendererType::OpenGL:
						shader_path = "shaders/glsl/";
						break;

					case bgfx::RendererType::OpenGLES:
						shader_path = "shaders/gles/";
						break;

					default:
						break;
				}

				shader_path.append(filename);
				shader_path.append(".bin");

				logger::log(shader_path);
				const bgfx::Memory* mem = get_bgfx_mem(shader_path);

				bgfx::ShaderHandle s = bgfx::createShader(mem);
				
				return s;
			}

			static bgfx::ProgramHandle load_program(const std::string& vs_name, const std::string& fs_name)
			{
				bgfx::ShaderHandle vsh = load_shader(vs_name);
				bgfx::ShaderHandle fsh = load_shader(fs_name);
				return bgfx::createProgram(vsh, fsh, true);
			}
	};
}

#pragma once

#include <string>

#include <noob/component/component.hpp>

#include "MathFuncs.hpp"
#include "BasicMesh.hpp"
#include "Model.hpp"
#include "GpuBuffer.hpp"

namespace noob
{
	class graphics
	{

		protected:

			// Static getter-related stuff.
			// TODO: Find cleaner way to do this.
			static graphics* ptr_to_instance;

			graphics() noexcept(true) {}

			graphics(const graphics& rhs) noexcept(true)
			{
				ptr_to_instance = rhs.ptr_to_instance;
			}

			graphics& operator=(const graphics& rhs) noexcept(true)
			{
				if (this != &rhs)
				{
					ptr_to_instance = rhs.ptr_to_instance;
				}
				return *this;
			}

			~graphics() noexcept(true) {}

		public:
			static graphics& get_instance() noexcept(true)
			{
				static graphics the_instance;
				ptr_to_instance = &the_instance;

				return *ptr_to_instance;
			}

			typedef noob::handle<uint32_t> shader_handle;
			typedef noob::handle<uint32_t> program_handle;

			class attrib
			{
				friend class graphics;

				public:

				enum class unit_type
				{
					HALF_FLOAT, FLOAT, INT16, INT32, UINT16, UINT32
				};

				enum class packing_type
				{
					VEC4, MAT4
				};

				noob::graphics::attrib::unit_type get_unit_type() const noexcept(true)
				{
					return unit;
				}

				noob::graphics::attrib::packing_type get_packing_type() const noexcept(true)
				{
					return packing;
				}

				protected:
				noob::graphics::attrib::unit_type unit;
				noob::graphics::attrib::packing_type packing;
			};

			class texture
			{
				friend class graphics;

				public:

				enum class storage_type
				{
					TEX_1D, TEX_2D_ARRAY, TEX_3D, TEX_CUBE
				};

				enum class compression_type
				{
					NONE, ETC2, PVRTC, ADST
				};

				noob::graphics::texture::storage_type get_storage_type() const noexcept(true)
				{
					return storage;
				}

				noob::graphics::texture::compression_type get_compression_type() const noexcept(true)
				{
					return compression;
				}

				protected:
				uint32_t handle;
				noob::graphics::texture::storage_type storage;
				noob::graphics::texture::compression_type compression;
			};

			typedef noob::handle<uint32_t> texture_handle;

			void init(uint32_t width, uint32_t height) noexcept(true);

			void destroy() noexcept(true);

			noob::model_handle model(noob::model::geom_type geom, const noob::basic_mesh&) noexcept(true);

			noob::model_handle model_instanced(const noob::basic_mesh&, const std::vector<noob::model::info>&) noexcept(true);

			noob::graphics::texture_handle reserve_textures_2d(uint32_t width, uint32_t height, uint32_t slots, uint32_t mips, noob::graphics::attrib::unit_type, noob::graphics::texture::compression_type) noexcept(true);

			noob::graphics::texture_handle texture_3d(uint32_t width, uint32_t height, uint32_t mips, noob::graphics::attrib::unit_type, noob::graphics::texture::compression_type, const std::string&) noexcept(true);	

			noob::graphics::texture_handle texture_cube(uint32_t width, uint32_t height, uint32_t mips, noob::graphics::attrib::unit_type, noob::graphics::texture::compression_type, const std::string&) noexcept(true);	

			void set_view_transform(const noob::mat4& view, const noob::mat4& proj) noexcept(true);

			void draw(const noob::model&, uint32_t num) noexcept(true);

			void frame(uint32_t width, uint32_t height) noexcept(true);

			std::tuple<bool, noob::gpu_write_buffer> get_buffer(const noob::model& m) noexcept(true);
			
			// NOTE: MUST be called as soon as you're finished using the buffer!
			void unmap_buffer(const noob::model& m) noexcept(true);

		protected:


			noob::component<noob::model> models;
			rde::vector<noob::graphics::texture> textures;

			// glDrawElementsInstanced ( GL_TRIANGLES, userData->numIndices, GL_UNSIGNED_INT, ( const void * ) NULL, NUM_INSTANCES );
			std::vector<noob::vec4> colour_storage;
			std::vector<noob::mat4> matrices_storage;

			noob::mat4 view_mat, proj_mat;
	};
}

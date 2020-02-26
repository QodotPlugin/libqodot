#include <gdnative_api_struct.gen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "godot_util.h"
#include "map_data.h"
#include "map_parser.h"
#include "geo_generator.h"
#include "surface_gatherer.h"

typedef struct user_data_struct
{
	char data[256];
} user_data_struct;

const godot_gdnative_core_api_struct *api = NULL;
const godot_gdnative_ext_nativescript_api_struct *nativescript_api = NULL;

GDCALLINGCONV void *qodot_constructor(godot_object *p_instance, void *p_method_data);
GDCALLINGCONV void qodot_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data);

godot_variant qodot_load_map(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant qodot_get_texture_list(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant qodot_set_entity_definitions(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant qodot_set_worldspawn_layers(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant qodot_generate_geometry(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant qodot_get_entity_dicts(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant qodot_get_worldspawn_layer_dicts(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant qodot_gather_texture_surfaces(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant qodot_gather_convex_collision_surfaces(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant qodot_gather_concave_collision_surfaces(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);
godot_variant qodot_fetch_surfaces(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args);

void GDN_EXPORT godot_gdnative_init(godot_gdnative_init_options *p_options)
{
	api = p_options->api_struct;

	// now find our extensions
	for (unsigned int i = 0; i < api->num_extensions; i++)
	{
		switch (api->extensions[i]->type)
		{
		case GDNATIVE_EXT_NATIVESCRIPT:
		{
			nativescript_api = (godot_gdnative_ext_nativescript_api_struct *)api->extensions[i];
		};
		break;
		default:
			break;
		};
	};
}

void GDN_EXPORT godot_gdnative_terminate(godot_gdnative_terminate_options *p_options)
{
	api = NULL;
	nativescript_api = NULL;
}

void GDN_EXPORT godot_nativescript_init(void *p_handle)
{
	godot_instance_create_func create = {NULL, NULL, NULL};
	create.create_func = &qodot_constructor;

	godot_instance_destroy_func destroy = {NULL, NULL, NULL};
	destroy.destroy_func = &qodot_destructor;

	nativescript_api->godot_nativescript_register_class(p_handle, "Qodot", "Reference", create, destroy);

	godot_method_attributes attributes = {GODOT_METHOD_RPC_MODE_DISABLED};

	GD_REGISTER_METHOD("Qodot", load_map, qodot_load_map);
	GD_REGISTER_METHOD("Qodot", get_texture_list, qodot_get_texture_list);
	GD_REGISTER_METHOD("Qodot", set_entity_definitions, qodot_set_entity_definitions);
	GD_REGISTER_METHOD("Qodot", set_worldspawn_layers, qodot_set_worldspawn_layers);
	GD_REGISTER_METHOD("Qodot", generate_geometry, qodot_generate_geometry);
	GD_REGISTER_METHOD("Qodot", get_entity_dicts, qodot_get_entity_dicts);
	GD_REGISTER_METHOD("Qodot", get_worldspawn_layer_dicts, qodot_get_worldspawn_layer_dicts);
	GD_REGISTER_METHOD("Qodot", gather_texture_surfaces, qodot_gather_texture_surfaces);
	GD_REGISTER_METHOD("Qodot", gather_convex_collision_surfaces, qodot_gather_convex_collision_surfaces);
	GD_REGISTER_METHOD("Qodot", gather_concave_collision_surfaces, qodot_gather_concave_collision_surfaces);
	GD_REGISTER_METHOD("Qodot", fetch_surfaces, qodot_fetch_surfaces);
}

GDCALLINGCONV void *qodot_constructor(godot_object *p_instance, void *p_method_data)
{
	user_data_struct *user_data = api->godot_alloc(sizeof(user_data_struct));

	return user_data;
}

GDCALLINGCONV void qodot_destructor(godot_object *p_instance, void *p_method_data, void *p_user_data)
{
	api->godot_free(p_user_data);
}

godot_variant qodot_load_map(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant *map_file_var = p_args[0];
	godot_string map_file_str = api->godot_variant_as_string(map_file_var);
	api->godot_variant_destroy(map_file_var);
	godot_char_string map_file_char_str = api->godot_string_utf8(&map_file_str);
	api->godot_string_destroy(&map_file_str);
	const char *map_file = api->godot_char_string_get_data(&map_file_char_str);
	api->godot_char_string_destroy(&map_file_char_str);

	map_parser_load(map_file);

	GD_RETURN_NULL();
}

godot_variant qodot_get_texture_list(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_pool_string_array g_textures;
	api->godot_pool_string_array_new(&g_textures);

	int tex_count = map_data_get_texture_count();
	texture_data *textures = map_data_get_textures();
	for (int t = 0; t < tex_count; ++t)
	{
		texture_data *texture = &textures[t];
		godot_string g_name;
		api->godot_string_new(&g_name);
		api->godot_string_parse_utf8(&g_name, texture->name);
		api->godot_pool_string_array_append(&g_textures, &g_name);
		api->godot_string_destroy(&g_name);
	}

	godot_variant g_textures_var;
	api->godot_variant_new_pool_string_array(&g_textures_var, &g_textures);
	api->godot_pool_string_array_destroy(&g_textures);

	return g_textures_var;
}

godot_variant qodot_set_entity_definitions(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant *g_entity_defs_var = p_args[0];
	godot_dictionary g_entity_defs = api->godot_variant_as_dictionary(g_entity_defs_var);
	godot_array g_keys = api->godot_dictionary_keys(&g_entity_defs);
	godot_array g_values = api->godot_dictionary_values(&g_entity_defs);
	api->godot_dictionary_destroy(&g_entity_defs);

	int entity_def_count = api->godot_array_size(&g_keys);
	for (int i = 0; i < entity_def_count; ++i)
	{
		godot_variant g_key_var = api->godot_array_get(&g_keys, i);
		const char *key;
		GD_VARIANT_STRING_TO_CHAR(key, g_key_var);

		godot_variant g_value_var = api->godot_array_get(&g_values, i);
		godot_dictionary g_value = api->godot_variant_as_dictionary(&g_value_var);
		api->godot_variant_destroy(&g_value_var);

		godot_string g_spawn_type_key;
		api->godot_string_new(&g_spawn_type_key);
		api->godot_string_parse_utf8(&g_spawn_type_key, "spawn_type");

		godot_variant g_spawn_type_key_var;
		api->godot_variant_new_string(&g_spawn_type_key_var, &g_spawn_type_key);
		api->godot_string_destroy(&g_spawn_type_key);

		godot_variant g_spawn_type_value_var = api->godot_dictionary_get(&g_value, &g_spawn_type_key_var);
		api->godot_dictionary_destroy(&g_value);
		api->godot_variant_destroy(&g_spawn_type_key_var);

		int64_t spawn_type = api->godot_variant_as_int(&g_spawn_type_value_var);
		api->godot_variant_destroy(&g_spawn_type_value_var);

		map_data_set_spawn_type_by_classname(key, (enum entity_spawn_type)spawn_type);
	}

	GD_RETURN_NULL();
}

godot_variant qodot_set_worldspawn_layers(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant *g_worldspawn_layers_var = p_args[0];
	godot_array g_worldspawn_layers = api->godot_variant_as_array(g_worldspawn_layers_var);

	int worldspawn_layer_count = api->godot_array_size(&g_worldspawn_layers);
	for (int l = 0; l < worldspawn_layer_count; l++)
	{
		godot_variant g_worldspawn_layer_var = api->godot_array_get(&g_worldspawn_layers, l);
		godot_dictionary g_worldspawn_layer = api->godot_variant_as_dictionary(&g_worldspawn_layer_var);
		api->godot_variant_destroy(&g_worldspawn_layer_var);

		godot_array g_keys = api->godot_dictionary_keys(&g_worldspawn_layer);
		godot_array g_values = api->godot_dictionary_values(&g_worldspawn_layer);
		api->godot_dictionary_destroy(&g_worldspawn_layer);

		int key_count = api->godot_array_size(&g_keys);

		const char *texture = NULL;
		bool build_visuals = false;

		for (int k = 0; k < key_count; ++k)
		{
			godot_variant g_key_var = api->godot_array_get(&g_keys, k);
			godot_variant g_value_var = api->godot_array_get(&g_values, k);

			const char *key;
			GD_VARIANT_STRING_TO_CHAR(key, g_key_var);

			if (strcmp(key, "texture") == 0)
			{
				godot_string g_texture = api->godot_variant_as_string(&g_value_var);
				GD_STRING_TO_CHAR(texture, g_texture);
			}
			else if (strcmp(key, "build_visuals") == 0)
			{
				build_visuals = api->godot_variant_as_bool(&g_value_var);
			}
		}

		map_data_register_worldspawn_layer(texture, build_visuals);
	}

	api->godot_array_destroy(&g_worldspawn_layers);

	GD_RETURN_NULL();
}

godot_variant qodot_generate_geometry(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant g_texture_dict_var = *p_args[0];
	godot_dictionary g_texture_dict = api->godot_variant_as_dictionary(&g_texture_dict_var);

	godot_array texture_keys = api->godot_dictionary_keys(&g_texture_dict);
	int texture_key_count = api->godot_array_size(&texture_keys);

	for (int i = 0; i < texture_key_count; ++i)
	{
		godot_variant texture_key_var = api->godot_array_get(&texture_keys, i);
		godot_string texture_key_string = api->godot_variant_as_string(&texture_key_var);
		godot_char_string texture_key_chars = api->godot_string_utf8(&texture_key_string);
		api->godot_string_destroy(&texture_key_string);
		const char *texture_key = api->godot_char_string_get_data(&texture_key_chars);
		api->godot_char_string_destroy(&texture_key_chars);

		godot_variant value_var = api->godot_dictionary_get(&g_texture_dict, &texture_key_var);
		api->godot_variant_destroy(&texture_key_var);

		godot_vector2 value = api->godot_variant_as_vector2(&value_var);

		int width = (int)api->godot_vector2_get_x(&value);
		int height = (int)api->godot_vector2_get_y(&value);

		map_data_set_texture_size(texture_key, width, height);
	}

	geo_generator_run();
	GD_RETURN_NULL();
}

godot_variant qodot_get_entity_dicts(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args)
{
	int ent_count = map_data_get_entity_count();
	const entity *ents = map_data_get_entities();

	godot_array g_edicts;
	api->godot_array_new(&g_edicts);

	for (int e = 0; e < ent_count; ++e)
	{
		const entity *ent = &ents[e];

		// Main entity dictionary
		godot_dictionary g_edict;
		api->godot_dictionary_new(&g_edict);

		// Brush count key
		godot_string g_brush_count_key;
		GD_STRING_FROM_UTF8(g_brush_count_key, "brush_count");

		godot_variant g_brush_count_key_var;
		GD_VARIANT_FROM_STRING(g_brush_count_key_var, g_brush_count_key);

		// Brush count value
		godot_variant g_brush_count_value_var;
		api->godot_variant_new_int(&g_brush_count_value_var, ent->brush_count);

		// Add brush count to dict
		GD_DICTIONARY_SET(g_edict, g_brush_count_key_var, g_brush_count_value_var);

		// Brush indices key
		godot_string g_brush_indices_key;
		GD_STRING_FROM_UTF8(g_brush_indices_key, "brush_indices");

		godot_variant g_brush_indices_key_var;
		GD_VARIANT_FROM_STRING(g_brush_indices_key_var, g_brush_indices_key);

		// Brush indices value
		godot_pool_int_array g_brush_indices;
		api->godot_pool_int_array_new(&g_brush_indices);

		for (int b = 0; b < ent->brush_count; ++b)
		{
			const brush *brush = &ent->brushes[b];
			bool is_worldspawn_layer_brush = false;

			for (int f = 0; f < brush->face_count; ++f)
			{
				const face *face = &brush->faces[f];

				if (map_data_find_worldspawn_layer(face->texture_idx) != -1)
				{
					is_worldspawn_layer_brush = true;
					break;
				}
			}

			if(!is_worldspawn_layer_brush)
			{
				api->godot_pool_int_array_append(&g_brush_indices, b);
			}
		}

		godot_variant g_brush_indices_value_var;
		api->godot_variant_new_pool_int_array(&g_brush_indices_value_var, &g_brush_indices);
		api->godot_pool_int_array_destroy(&g_brush_indices);

		// Add brush indices to dict
		GD_DICTIONARY_SET(g_edict, g_brush_indices_key_var, g_brush_indices_value_var);

		// Center key
		godot_string g_center_key;
		GD_STRING_FROM_UTF8(g_center_key, "center");

		godot_variant g_center_key_var;
		GD_VARIANT_FROM_STRING(g_center_key_var, g_center_key);

		// Center value
		godot_vector3 g_center_value;
		api->godot_vector3_new(&g_center_value, (godot_real)ent->center.y, (godot_real)ent->center.z, (godot_real)ent->center.x);
		godot_variant g_center_value_var;
		api->godot_variant_new_vector3(&g_center_value_var, &g_center_value);

		// Add center to dict
		GD_DICTIONARY_SET(g_edict, g_center_key_var, g_center_value_var);

		// Property dict key
		godot_string g_properties_key;
		GD_STRING_FROM_UTF8(g_properties_key, "properties");

		godot_variant g_properties_key_var;
		GD_VARIANT_FROM_STRING(g_properties_key_var, g_properties_key);

		// Property dict value
		godot_dictionary g_properties_value;
		api->godot_dictionary_new(&g_properties_value);

		for (int p = 0; p < ent->property_count; ++p)
		{
			property *prop = &ent->properties[p];

			// Key
			godot_string g_key;
			GD_STRING_FROM_UTF8(g_key, prop->key);

			godot_variant g_key_var;
			GD_VARIANT_FROM_STRING(g_key_var, g_key)

			// Value
			godot_string g_value;
			GD_STRING_FROM_UTF8(g_value, prop->value);

			godot_variant g_value_var;
			GD_VARIANT_FROM_STRING(g_value_var, g_value)

			// Add to dictionary
			GD_DICTIONARY_SET(g_properties_value, g_key_var, g_value_var);
		}

		godot_variant g_properties_value_var;
		api->godot_variant_new_dictionary(&g_properties_value_var, &g_properties_value);

		// Add properties to dict
		GD_DICTIONARY_SET(g_edict, g_properties_key_var, g_properties_value_var);

		// Add dictionary to array
		godot_variant g_edict_var;
		api->godot_variant_new_dictionary(&g_edict_var, &g_edict);
		api->godot_dictionary_destroy(&g_edict);

		api->godot_array_append(&g_edicts, &g_edict_var);
		api->godot_variant_destroy(&g_edict_var);
	}

	godot_variant g_edicts_var;
	api->godot_variant_new_array(&g_edicts_var, &g_edicts);
	api->godot_array_destroy(&g_edicts);

	return g_edicts_var;
}

godot_variant qodot_get_worldspawn_layer_dicts(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args)
{
	int ent_count = map_data_get_entity_count();
	const entity *ents = map_data_get_entities();
	const entity *worldspawn_entity = &ents[0];

	if(worldspawn_entity == NULL)
	{
		GD_RETURN_NULL();
	}

	godot_array g_layer_dicts;
	api->godot_array_new(&g_layer_dicts);

	int layer_count = map_data_get_worldspawn_layer_count();
	const worldspawn_layer *layers = map_data_get_worldspawn_layers();

	for (int l = 0; l < layer_count; ++l)
	{
		const worldspawn_layer *worldspawn_layer = &layers[l];

		// Main layer dictionary
		godot_dictionary g_layer_dict;
		api->godot_dictionary_new(&g_layer_dict);

		// Texture key
		godot_string g_texture_key;
		GD_STRING_FROM_UTF8(g_texture_key, "texture");

		godot_variant g_texture_key_var;
		GD_VARIANT_FROM_STRING(g_texture_key_var, g_texture_key);

		// Texture value
		godot_string g_texture_value;

		texture_data *tex_data = map_data_get_texture(worldspawn_layer->texture_idx);
		if(tex_data == NULL)
		{
			continue;
		}

		GD_STRING_FROM_UTF8(g_texture_value, tex_data->name);

		godot_variant g_texture_value_var;
		GD_VARIANT_FROM_STRING(g_texture_value_var, g_texture_value);

		// Add texture to dict
		GD_DICTIONARY_SET(g_layer_dict, g_texture_key_var, g_texture_value_var);

		// Brush indices key
		godot_string g_brush_indices_key;
		GD_STRING_FROM_UTF8(g_brush_indices_key, "brush_indices");

		godot_variant g_brush_indices_key_var;
		GD_VARIANT_FROM_STRING(g_brush_indices_key_var, g_brush_indices_key);

		// Brush indices value
		godot_pool_int_array g_brush_indices_value;
		api->godot_pool_int_array_new(&g_brush_indices_value);

		for (int b = 0; b < worldspawn_entity->brush_count; ++b)
		{
			const brush *brush = &worldspawn_entity->brushes[b];
			bool is_layer_brush = false;

			for (int f = 0; f < brush->face_count; ++f)
			{
				const face *face = &brush->faces[f];

				if (map_data_find_worldspawn_layer(face->texture_idx) != -1)
				{
					is_layer_brush = true;
					break;
				}
			}

			if(is_layer_brush)
			{
				api->godot_pool_int_array_append(&g_brush_indices_value, b);
			}
		}

		godot_variant g_brush_indices_value_var;
		api->godot_variant_new_pool_int_array(&g_brush_indices_value_var, &g_brush_indices_value);

		// Add brush indices to dict
		GD_DICTIONARY_SET(g_layer_dict, g_brush_indices_key_var, g_brush_indices_value_var);

		// Add dictionary to array
		godot_variant g_layer_dict_var;
		api->godot_variant_new_dictionary(&g_layer_dict_var, &g_layer_dict);
		api->godot_dictionary_destroy(&g_layer_dict);

		api->godot_array_append(&g_layer_dicts, &g_layer_dict_var);
		api->godot_variant_destroy(&g_layer_dict_var);
	}

	godot_variant g_layer_dicts_var;
	api->godot_variant_new_array(&g_layer_dicts_var, &g_layer_dicts);
	api->godot_array_destroy(&g_layer_dicts);

	return g_layer_dicts_var;
}

godot_variant qodot_gather_texture_surfaces(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args)
{
	GD_VARIANT_ARGV(string, g_texture_name, 0);
	GD_VARIANT_ARGV(string, g_brush_filter_texture, 1);
	GD_VARIANT_ARGV(string, g_face_filter_texture, 2);

	const char *texture_name = NULL;
	const char *brush_filter_texture = NULL;
	const char *face_filter_texture = NULL;

	GD_STRING_TO_CHAR(texture_name, g_texture_name)
	GD_STRING_TO_CHAR(brush_filter_texture, g_brush_filter_texture)
	GD_STRING_TO_CHAR(face_filter_texture, g_face_filter_texture)

	surface_gatherer_reset_params();
	surface_gatherer_set_split_type(SST_ENTITY);
	surface_gatherer_set_texture_filter(texture_name);
	surface_gatherer_set_brush_filter_texture(brush_filter_texture);
	surface_gatherer_set_face_filter_texture(face_filter_texture);

	surface_gatherer_run();

	GD_RETURN_NULL();
}

godot_variant qodot_gather_convex_collision_surfaces(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant g_entity_idx_var = *p_args[0];
	int64_t entity_idx = api->godot_variant_as_int(&g_entity_idx_var);

	surface_gatherer_reset_params();
	surface_gatherer_set_split_type(SST_BRUSH);
	surface_gatherer_set_entity_index_filter((int)entity_idx);

	surface_gatherer_run();

	GD_RETURN_NULL();
}

godot_variant qodot_gather_concave_collision_surfaces(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args)
{
	godot_variant g_entity_idx_var = *p_args[0];
	int64_t entity_idx = api->godot_variant_as_int(&g_entity_idx_var);

	surface_gatherer_reset_params();
	surface_gatherer_set_split_type(SST_NONE);
	surface_gatherer_set_entity_index_filter((int)entity_idx);

	surface_gatherer_run();

	GD_RETURN_NULL();
}

godot_variant qodot_fetch_surfaces(godot_object *p_instance, void *p_method_data, void *p_user_data, int p_num_args, godot_variant **p_args)
{
	double inverse_scale_factor = api->godot_variant_as_real(p_args[0]);

	// Null variant for unused arrays
	godot_variant g_nil;
	api->godot_variant_new_nil(&g_nil);

	// Temporary vectors
	godot_vector2 gv2;
	godot_vector3 gv3;

	const surfaces *surfs = surface_gatherer_fetch();

	godot_array surf_array;
	api->godot_array_new(&surf_array);

	for (int s = 0; s < surfs->surface_count; ++s)
	{
		surface *surf = &surfs->surfaces[s];

		if (surf->vertex_count == 0)
		{
			api->godot_array_append(&surf_array, &g_nil);
			continue;
		}

		// Create vertex array
		godot_pool_vector3_array g_vertices;
		api->godot_pool_vector3_array_new(&g_vertices);

		for (int v = 0; v < surf->vertex_count; ++v)
		{
			api->godot_vector3_new(&gv3, (godot_real)surf->vertices[v].vertex.y, (godot_real)surf->vertices[v].vertex.z, (godot_real)surf->vertices[v].vertex.x);
			gv3 = api->godot_vector3_operator_divide_scalar(&gv3, (godot_real)inverse_scale_factor);
			api->godot_pool_vector3_array_append(&g_vertices, &gv3);
		}

		godot_variant g_vertices_var;
		api->godot_variant_new_pool_vector3_array(&g_vertices_var, &g_vertices);
		api->godot_pool_vector3_array_destroy(&g_vertices);

		// Create normal array
		godot_pool_vector3_array g_normals;
		api->godot_pool_vector3_array_new(&g_normals);

		for (int v = 0; v < surf->vertex_count; ++v)
		{
			api->godot_vector3_new(&gv3, (godot_real)surf->vertices[v].normal.y, (godot_real)surf->vertices[v].normal.z, (godot_real)surf->vertices[v].normal.x);
			api->godot_pool_vector3_array_append(&g_normals, &gv3);
		}

		godot_variant g_normals_var;
		api->godot_variant_new_pool_vector3_array(&g_normals_var, &g_normals);
		api->godot_pool_vector3_array_destroy(&g_normals);

		// Create tangent array
		godot_pool_real_array g_tangents;
		api->godot_pool_real_array_new(&g_tangents);

		for (int v = 0; v < surf->vertex_count; ++v)
		{
			api->godot_pool_real_array_append(&g_tangents, (godot_real)surf->vertices[v].tangent.y);
			api->godot_pool_real_array_append(&g_tangents, (godot_real)surf->vertices[v].tangent.z);
			api->godot_pool_real_array_append(&g_tangents, (godot_real)surf->vertices[v].tangent.x);
			api->godot_pool_real_array_append(&g_tangents, (godot_real)surf->vertices[v].tangent.w);
		}

		godot_variant g_tangents_var;
		api->godot_variant_new_pool_real_array(&g_tangents_var, &g_tangents);
		api->godot_pool_real_array_destroy(&g_tangents);

		// Create UV array
		godot_pool_vector2_array g_uvs;
		api->godot_pool_vector2_array_new(&g_uvs);

		for (int v = 0; v < surf->vertex_count; ++v)
		{
			api->godot_vector2_new(&gv2, (godot_real)surf->vertices[v].uv.u, (godot_real)surf->vertices[v].uv.v);
			api->godot_pool_vector2_array_append(&g_uvs, &gv2);
		}

		godot_variant g_uvs_var;
		api->godot_variant_new_pool_vector2_array(&g_uvs_var, &g_uvs);
		api->godot_pool_vector2_array_destroy(&g_uvs);

		// Create index array
		godot_pool_int_array g_indices;
		api->godot_pool_int_array_new(&g_indices);
		for (int i = 0; i < surf->index_count; ++i)
		{
			api->godot_pool_int_array_append(&g_indices, surf->indices[i]);
		}

		godot_variant g_indices_var;
		api->godot_variant_new_pool_int_array(&g_indices_var, &g_indices);
		api->godot_pool_int_array_destroy(&g_indices);

		// Build vertex arrays
		godot_array brush_array;
		api->godot_array_new(&brush_array);

		api->godot_array_append(&brush_array, &g_vertices_var);
		api->godot_array_append(&brush_array, &g_normals_var);
		api->godot_array_append(&brush_array, &g_tangents_var);
		api->godot_array_append(&brush_array, &g_nil);
		api->godot_array_append(&brush_array, &g_uvs_var);
		api->godot_array_append(&brush_array, &g_nil);
		api->godot_array_append(&brush_array, &g_nil);
		api->godot_array_append(&brush_array, &g_nil);
		api->godot_array_append(&brush_array, &g_indices_var);

		api->godot_variant_destroy(&g_vertices_var);
		api->godot_variant_destroy(&g_normals_var);
		api->godot_variant_destroy(&g_tangents_var);
		api->godot_variant_destroy(&g_uvs_var);
		api->godot_variant_destroy(&g_indices_var);
		api->godot_variant_destroy(&g_nil);

		// Store vertex arrays in surface array
		godot_variant brush_array_var;
		api->godot_variant_new_array(&brush_array_var, &brush_array);
		api->godot_array_destroy(&brush_array);

		api->godot_array_append(&surf_array, &brush_array_var);
		api->godot_variant_destroy(&brush_array_var);
	}

	// Store surface array in output array
	godot_variant surf_array_var;
	api->godot_variant_new_array(&surf_array_var, &surf_array);
	api->godot_array_destroy(&surf_array);

	return surf_array_var;
}

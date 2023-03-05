/*
 * Descriptors index functions
 *
 * Copyright (C) 2008-2023, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <memory.h>
#include <types.h>

#include "libpff_definitions.h"
#include "libpff_descriptors_index.h"
#include "libpff_index.h"
#include "libpff_index_value.h"
#include "libpff_index_values_list.h"
#include "libpff_io_handle.h"
#include "libpff_libbfio.h"
#include "libpff_libcdata.h"
#include "libpff_libcerror.h"
#include "libpff_libfcache.h"
#include "libpff_libfdata.h"

/* Creates a descriptors index
 * Make sure the value descriptors_index is referencing, is set to NULL
 * Returns 1 if successful or -1 on error
 */
int libpff_descriptors_index_initialize(
     libpff_descriptors_index_t **descriptors_index,
     libpff_io_handle_t *io_handle,
     libfdata_vector_t *index_nodes_vector,
     libfcache_cache_t *index_nodes_cache,
     off64_t root_node_offset,
     uint64_t root_node_back_pointer,
     libcerror_error_t **error )
{
	static char *function = "libpff_descriptors_index_initialize";

	if( descriptors_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid descriptors index.",
		 function );

		return( -1 );
	}
	if( *descriptors_index != NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid descriptors index value already set.",
		 function );

		return( -1 );
	}
	if( io_handle == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid IO handle.",
		 function );

		return( -1 );
	}
	*descriptors_index = memory_allocate_structure(
	                      libpff_descriptors_index_t );

	if( *descriptors_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create descriptors index.",
		 function );

		goto on_error;
	}
	if( memory_set(
	     *descriptors_index,
	     0,
	     sizeof( libpff_descriptors_index_t ) ) == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_MEMORY,
		 LIBCERROR_MEMORY_ERROR_SET_FAILED,
		 "%s: unable to clear descriptors index.",
		 function );

		memory_free(
		 *descriptors_index );

		*descriptors_index = NULL;

		return( -1 );
	}
	if( libpff_index_initialize(
	     &( ( *descriptors_index )->index ),
	     index_nodes_vector,
	     index_nodes_cache,
	     LIBPFF_INDEX_TYPE_DESCRIPTOR,
	     root_node_offset,
	     root_node_back_pointer,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create index.",
		 function );

		goto on_error;
	}
	if( libcdata_btree_initialize(
	     &( ( *descriptors_index )->recovered_index_values_tree ),
	     257,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create recovered index values B-tree.",
		 function );

		goto on_error;
	}
	if( libfcache_cache_initialize(
	     &( ( *descriptors_index )->index_cache ),
	     LIBPFF_MAXIMUM_CACHE_ENTRIES_DESCRIPTOR_INDEX_VALUES,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create index values cache.",
		 function );

		goto on_error;
	}
	( *descriptors_index )->io_handle          = io_handle;
	( *descriptors_index )->index_nodes_vector = index_nodes_vector;
	( *descriptors_index )->index_nodes_cache  = index_nodes_cache;

	return( 1 );

on_error:
	if( *descriptors_index != NULL )
	{
		if( ( *descriptors_index )->recovered_index_values_tree != NULL )
		{
			libcdata_btree_free(
			 &( ( *descriptors_index )->recovered_index_values_tree ),
			 (int (*)(intptr_t **, libcerror_error_t **)) &libpff_index_values_list_free,
			 NULL );
		}
		if( ( *descriptors_index )->index != NULL )
		{
			libpff_index_free(
			 &( ( *descriptors_index )->index ),
			 NULL );
		}
		memory_free(
		 *descriptors_index );

		*descriptors_index = NULL;
	}
	return( -1 );
}

/* Frees a descriptors index
 * Returns 1 if successful or -1 on error
 */
int libpff_descriptors_index_free(
     libpff_descriptors_index_t **descriptors_index,
     libcerror_error_t **error )
{
	static char *function = "libpff_descriptors_index_free";
	int result            = 1;

	if( descriptors_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid descriptors index.",
		 function );

		return( -1 );
	}
	if( *descriptors_index != NULL )
	{
		if( libfcache_cache_free(
		     &( ( *descriptors_index )->index_cache ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free index values cache.",
			 function );

			result = -1;
		}
		if( libcdata_btree_free(
		     &( ( *descriptors_index )->recovered_index_values_tree ),
		     (int (*)(intptr_t **, libcerror_error_t **)) &libpff_index_values_list_free,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free recovered index values tree.",
			 function );

			result = -1;
		}
		if( libpff_index_free(
		     &( ( *descriptors_index )->index ),
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free index.",
			 function );

			result = -1;
		}
		memory_free(
		 *descriptors_index );

		*descriptors_index = NULL;
	}
	return( result );
}

/* Retrieves an index value for a specific identifier
 * Returns 1 if successful, 0 if no index value was found or -1 on error
 */
int libpff_descriptors_index_get_index_value_by_identifier(
     libpff_descriptors_index_t *descriptors_index,
     libbfio_handle_t *file_io_handle,
     uint32_t descriptor_identifier,
     uint8_t recovered,
     libpff_index_value_t **index_value,
     libcerror_error_t **error )
{
	libcdata_tree_node_t *upper_node              = NULL;
	libpff_index_value_t *lookup_index_value      = NULL;
	libpff_index_value_t *safe_index_value        = NULL;
	libpff_index_values_list_t *index_values_list = NULL;
	static char *function                         = "libpff_descriptors_index_get_index_value_by_identifier";
	int result                                    = 0;

	if( descriptors_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid descriptors index.",
		 function );

		return( -1 );
	}
	if( index_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid index value.",
		 function );

		return( -1 );
	}
	if( recovered == 0 )
	{
		result = libpff_index_get_value_by_identifier(
			  descriptors_index->index,
			  descriptors_index->io_handle,
			  file_io_handle,
			  descriptor_identifier,
			  index_value,
			  error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve index value: %" PRIu32 " from index.",
			 function,
			 descriptor_identifier );

			goto on_error;
		}
	}
	else
	{
		if( libpff_index_value_initialize(
		     &lookup_index_value,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to create lookup index value.",
			 function );

			goto on_error;
		}
		lookup_index_value->identifier = descriptor_identifier;

		result = libcdata_btree_get_value_by_value(
		          descriptors_index->recovered_index_values_tree,
		          (intptr_t *) lookup_index_value,
		          (int (*)(intptr_t *, intptr_t *, libcerror_error_t **)) &libpff_index_values_list_compare,
		          &upper_node,
		          (intptr_t **) &index_values_list,
		          error );

		if( result == -1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve recovered index value: %" PRIu32 " list from tree.",
			 function,
			 descriptor_identifier );

			goto on_error;
		}
		else if( result != 0 )
		{
			if( libpff_index_values_list_get_value_by_index(
			     index_values_list,
			     0,
			     &safe_index_value,
			     error ) != 1 )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve retrieve recovered index value: %" PRIu32 " list entry: 0.",
				 function,
				 descriptor_identifier );

				goto on_error;
			}
			if( safe_index_value == NULL )
			{
				libcerror_error_set(
				 error,
				 LIBCERROR_ERROR_DOMAIN_RUNTIME,
				 LIBCERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: missing recovered index value: %" PRIu32 " list entry: 0.",
				 function,
				 descriptor_identifier );

				goto on_error;
			}
			lookup_index_value->data_identifier              = lookup_index_value->data_identifier;
			lookup_index_value->local_descriptors_identifier = lookup_index_value->local_descriptors_identifier;
			lookup_index_value->parent_identifier            = lookup_index_value->parent_identifier;

			*index_value = lookup_index_value;
		}
	}
	return( result );

on_error:
	if( lookup_index_value != NULL )
	{
		libpff_index_value_free(
		 &lookup_index_value,
		 NULL );
	}
	return( -1 );
}

/* Inserts a recovered index value into the descriptors index
 * Returns 1 if successful or -1 on error
 */
int libpff_descriptors_index_insert_recovered_index_value(
     libpff_descriptors_index_t *descriptors_index,
     libpff_index_value_t *index_value,
     libcerror_error_t **error )
{
	libcdata_tree_node_t *upper_node                       = NULL;
	libpff_index_values_list_t *existing_index_values_list = NULL;
	libpff_index_values_list_t *index_values_list          = NULL;
	static char *function                                  = "libpff_descriptors_index_insert_recovered_index_value";
	int result                                             = 0;
	int value_index                                        = 0;

	if( descriptors_index == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid descriptors index.",
		 function );

		return( -1 );
	}
	if( index_value == NULL )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBCERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid index value.",
		 function );

		return( -1 );
	}
	if( libpff_index_values_list_initialize(
	     &index_values_list,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to create index values list.",
		 function );

		goto on_error;
	}
	index_values_list->identifier = index_value->identifier;

	result = libcdata_btree_insert_value(
	          descriptors_index->recovered_index_values_tree,
	          &value_index,
	          (intptr_t *) index_values_list,
	          (int (*)(intptr_t *, intptr_t *, libcerror_error_t **)) &libpff_index_values_list_compare,
	          &upper_node,
	          (intptr_t **) &existing_index_values_list,
	          error );

	if( result == -1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to insert recovered descriptors index value: %" PRIu64 " list.",
		 function,
		 index_value->identifier );

		goto on_error;
	}
	else if( result != 0 )
	{
		existing_index_values_list = index_values_list;
		index_values_list          = NULL;
	}
	else
	{
		if( libpff_index_values_list_free(
		     &index_values_list,
		     error ) != 1 )
		{
			libcerror_error_set(
			 error,
			 LIBCERROR_ERROR_DOMAIN_RUNTIME,
			 LIBCERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free index values list.",
			 function );

			goto on_error;
		}
	}
	if( libpff_index_values_list_append_value(
	     existing_index_values_list,
	     index_value,
	     error ) != 1 )
	{
		libcerror_error_set(
		 error,
		 LIBCERROR_ERROR_DOMAIN_RUNTIME,
		 LIBCERROR_RUNTIME_ERROR_APPEND_FAILED,
		 "%s: unable to append index value to list.",
		 function );

		goto on_error;
	}
	return( 1 );

on_error:
	if( index_values_list != NULL )
	{
		libpff_index_values_list_free(
		 &index_values_list,
		 NULL );
	}
	return( -1 );
}


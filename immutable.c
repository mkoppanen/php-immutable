/*
+-----------------------------------------------------------------------------------+
|  Immutable object                                                                 |
|  Copyright (c) 2010, Mikko Koppanen <mkoppanen@php.net>                           |
|  All rights reserved.                                                             |
+-----------------------------------------------------------------------------------+
|  Redistribution and use in source and binary forms, with or without               |
|  modification, are permitted provided that the following conditions are met:      |
|     * Redistributions of source code must retain the above copyright              |
|       notice, this list of conditions and the following disclaimer.               |
|     * Redistributions in binary form must reproduce the above copyright           |
|       notice, this list of conditions and the following disclaimer in the         |
|       documentation and/or other materials provided with the distribution.        |
|     * Neither the name of the copyright holder nor the                            |
|       names of its contributors may be used to endorse or promote products        |
|       derived from this software without specific prior written permission.       |
+-----------------------------------------------------------------------------------+
|  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND  |
|  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED    |
|  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE           |
|  DISCLAIMED. IN NO EVENT SHALL MIKKO KOPPANEN BE LIABLE FOR ANY                   |
|  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES       |
|  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;     |
|  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND      |
|  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT       |
|  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS    |
|  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                     |
+-----------------------------------------------------------------------------------+
*/

#include "php_immutable.h"

zend_class_entry *php_immutable_sc_entry;

static zend_object_handlers php_immutable_object_handlers;

PHP_METHOD(immutable, __construct) 
{
	zval *params, **ppzval;
	zend_object_handlers *std_hnd;
	php_immutable_object *intern;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "a", &params) == FAILURE) {
		return;
	}

	std_hnd = zend_get_std_object_handlers();
	
	for (zend_hash_internal_pointer_reset(Z_ARRVAL_P(params));
			zend_hash_get_current_data(Z_ARRVAL_P(params), (void **) &ppzval) == SUCCESS;
			zend_hash_move_forward(Z_ARRVAL_P(params))) {
	
		char *string_key;
		ulong num_key;
	
		if (zend_hash_get_current_key(Z_ARRVAL_P(params), &string_key, &num_key, 0) == HASH_KEY_IS_STRING) {
			zval *k;	
			
			MAKE_STD_ZVAL(k);
			ZVAL_STRING(k, string_key, 1);
			
			std_hnd->write_property(getThis(), k, *ppzval TSRMLS_CC);
			zval_ptr_dtor(&k);
		}
	}
}

static void php_immutable_object_free_storage(void *object TSRMLS_DC)
{
	php_immutable_object *intern = (php_immutable_object *)object;

	if (!intern) {
		return;
	}
	
	zend_hash_destroy(&(intern->properties));
	zend_object_std_dtor(&intern->zo TSRMLS_CC);
	efree(intern);
}

static void php_immutable_free_prop(zval ***ptr TSRMLS_DC)
{
	zval_ptr_dtor(*ptr);
	efree(*ptr);
}

static zend_object_value php_immutable_object_new(zend_class_entry *class_type TSRMLS_DC)
{
	zval *tmp;
	zend_object_value retval;
	php_immutable_object *intern;

	/* Allocate memory for it */
	intern = (php_immutable_object *) emalloc(sizeof(php_immutable_object));
	memset(&intern->zo, 0, sizeof(zend_object));

	zend_hash_init(&(intern->properties), 5, NULL, (void (*)(void *)) php_immutable_free_prop, 0);

	zend_object_std_init(&intern->zo, class_type TSRMLS_CC);
	zend_hash_copy(intern->zo.properties, &class_type->default_properties, (copy_ctor_func_t) zval_add_ref,(void *) &tmp, sizeof(zval *));

	retval.handle = zend_objects_store_put(intern, NULL, (zend_objects_free_object_storage_t) php_immutable_object_free_storage, NULL TSRMLS_CC);
	retval.handlers = (zend_object_handlers *) &php_immutable_object_handlers;
	return retval;
}

PHP_MINFO_FUNCTION(immutable)
{
}

static function_entry php_immutable_class_methods[] = {
	PHP_ME(immutable, __construct,			NULL,			ZEND_ACC_PUBLIC|ZEND_ACC_CTOR|ZEND_ACC_FINAL)	
	{NULL, NULL, NULL}
};

static zval **php_immutable_get_property_ptr_ptr(zval *object, zval *member TSRMLS_DC)
{
	zval **retval, **prop_ptr_ptr;
	php_immutable_object *intern;
	zend_object_handlers *std_hnd;

	intern = (php_immutable_object *) zend_objects_get_address(object TSRMLS_CC);

	/* Get the property */
	std_hnd = zend_get_std_object_handlers();
	retval  = std_hnd->get_property_ptr_ptr(object, member TSRMLS_CC);

	prop_ptr_ptr = emalloc(sizeof(zval *));

	MAKE_STD_ZVAL((*prop_ptr_ptr));
	ZVAL_ZVAL((*prop_ptr_ptr), (*retval), 1, 0);

	zend_hash_next_index_insert(&(intern->properties), (void *) &prop_ptr_ptr, sizeof(zval ***), NULL);
	return prop_ptr_ptr;
}

static void php_immutable_write_property(zval *object, zval *member, zval *value TSRMLS_DC) 
{
	/* noop */
}

PHP_MINIT_FUNCTION(immutable)
{
	zend_class_entry ce;
	
	memcpy(&php_immutable_object_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));

	INIT_CLASS_ENTRY(ce, "Immutable", php_immutable_class_methods);
	ce.create_object = php_immutable_object_new;
	php_immutable_object_handlers.clone_obj = NULL;

	php_immutable_object_handlers.write_property       = php_immutable_write_property;
	php_immutable_object_handlers.get_property_ptr_ptr = php_immutable_get_property_ptr_ptr;
	
	php_immutable_sc_entry = zend_register_internal_class(&ce TSRMLS_CC);
	return SUCCESS;
}

zend_function_entry immutable_functions[] = {
	{NULL, NULL, NULL} 
};

zend_module_entry immutable_module_entry =
{
        STANDARD_MODULE_HEADER,
        PHP_IMMUTABLE_EXTNAME,
        immutable_functions,			/* Functions */
        PHP_MINIT(immutable),			/* MINIT */
        NULL,							/* MSHUTDOWN */
        NULL,							/* RINIT */
        NULL,							/* RSHUTDOWN */
        PHP_MINFO(immutable),			/* MINFO */
        PHP_IMMUTABLE_EXTVER,			/* version */
        STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_IMMUTABLE
ZEND_GET_MODULE(immutable)
#endif /* COMPILE_DL_IMMUTABLE */
/*
 * Copyright (c) 2008-2012 Apple Inc. All rights reserved.
 *
 * @APPLE_APACHE_LICENSE_HEADER_START@
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * @APPLE_APACHE_LICENSE_HEADER_END@
 */

#ifndef __DISPATCH_OBJECT__
#define __DISPATCH_OBJECT__

#ifndef __DISPATCH_INDIRECT__
#error "Please #include <dispatch/dispatch.h> instead of this file directly."
#include <dispatch/base.h> // for HeaderDoc
#endif

#if __has_include(<sys/qos.h>)
#include <sys/qos.h>
#endif

DISPATCH_ASSUME_NONNULL_BEGIN
DISPATCH_ASSUME_ABI_SINGLE_BEGIN

/*!
 * @typedef dispatch_object_t
 *
 * @abstract
 * Abstract base type for all dispatch objects.
 * The details of the type definition are language-specific.
 *
 * @discussion
 * Dispatch objects are reference counted via calls to dispatch_retain() and
 * dispatch_release().
 */

#if OS_OBJECT_USE_OBJC
/*
 * By default, dispatch objects are declared as Objective-C types when building
 * with an Objective-C compiler. This allows them to participate in ARC, in RR
 * management by the Blocks runtime and in leaks checking by the static
 * analyzer, and enables them to be added to Cocoa collections.
 * See <os/object.h> for details.
 */
OS_OBJECT_DECL_CLASS(dispatch_object);

#if OS_OBJECT_SWIFT3
#define DISPATCH_DECL(name) OS_OBJECT_DECL_SUBCLASS_SWIFT(name, dispatch_object)
#define DISPATCH_DECL_SUBCLASS(name, base) OS_OBJECT_DECL_SUBCLASS_SWIFT(name, base)
#else // OS_OBJECT_SWIFT3
#define DISPATCH_DECL(name) OS_OBJECT_DECL_SUBCLASS(name, dispatch_object)
#define DISPATCH_DECL_SUBCLASS(name, base) OS_OBJECT_DECL_SUBCLASS(name, base)

DISPATCH_INLINE DISPATCH_ALWAYS_INLINE DISPATCH_NONNULL_ALL DISPATCH_NOTHROW
void
_dispatch_object_validate(dispatch_object_t object)
{
	void *isa = *(void *volatile*)(OS_OBJECT_BRIDGE void*)object;
	(void)isa;
}
#endif // OS_OBJECT_SWIFT3

#define DISPATCH_GLOBAL_OBJECT(type, object) ((OS_OBJECT_BRIDGE type)&(object))
#define DISPATCH_RETURNS_RETAINED OS_OBJECT_RETURNS_RETAINED
#elif defined(__cplusplus) && !defined(__DISPATCH_BUILDING_DISPATCH__)
/*
 * Dispatch objects are NOT C++ objects. Nevertheless, we can at least keep C++
 * aware of type compatibility.
 */
typedef struct dispatch_object_s {
private:
	dispatch_object_s();
	~dispatch_object_s();
	dispatch_object_s(const dispatch_object_s &);
	void operator=(const dispatch_object_s &);
} *dispatch_object_t;
#define DISPATCH_DECL(name) \
		typedef struct name##_s : public dispatch_object_s {} *name##_t
#define DISPATCH_DECL_SUBCLASS(name, base) \
		typedef struct name##_s : public base##_s {} *name##_t
#define DISPATCH_GLOBAL_OBJECT(type, object) (static_cast<type>(&(object)))
#define DISPATCH_RETURNS_RETAINED
#else /* Plain C */
typedef union {
	struct _os_object_s *_os_obj;
	struct dispatch_object_s *_do;
	struct dispatch_queue_s *_dq;
	struct dispatch_queue_attr_s *_dqa;
	struct dispatch_group_s *_dg;
	struct dispatch_source_s *_ds;
	struct dispatch_channel_s *_dch;
	struct dispatch_mach_s *_dm;
	struct dispatch_mach_msg_s *_dmsg;
	struct dispatch_semaphore_s *_dsema;
	struct dispatch_data_s *_ddata;
	struct dispatch_io_s *_dchannel;
} dispatch_object_t DISPATCH_TRANSPARENT_UNION;
#define DISPATCH_DECL(name) typedef struct name##_s *name##_t
#define DISPATCH_DECL_SUBCLASS(name, base) typedef base##_t name##_t
#define DISPATCH_GLOBAL_OBJECT(type, object) ((type)&(object))
#define DISPATCH_RETURNS_RETAINED
#endif

#if OS_OBJECT_SWIFT3 && OS_OBJECT_USE_OBJC
#define DISPATCH_SOURCE_TYPE_DECL(name) \
		DISPATCH_EXPORT struct dispatch_source_type_s \
				_dispatch_source_type_##name; \
		OS_OBJECT_DECL_PROTOCOL(dispatch_source_##name, <OS_dispatch_source>); \
		OS_OBJECT_CLASS_IMPLEMENTS_PROTOCOL( \
				dispatch_source, dispatch_source_##name)
#define DISPATCH_SOURCE_DECL(name) \
		DISPATCH_DECL(name); \
		OS_OBJECT_DECL_PROTOCOL(name, <NSObject>); \
		OS_OBJECT_CLASS_IMPLEMEN
#ifndef _ESVG_PRIVATE_ATTRIBUTE_CLIP_PATH_H_
#define _ESVG_PRIVATE_ATTRIBUTE_CLIP_PATH_H_

typedef struct _Esvg_Attribute_Clip_Path
{
	Esvg_Clip_Path v;
	Eina_Bool is_set;
} Esvg_Attribute_Clip_Path;

typedef struct _Esvg_Attribute_Animated_Clip_Path
{
	Esvg_Attribute_Clip_Path base;
	Esvg_Attribute_Clip_Path anim;
	int animated;
} Esvg_Attribute_Animated_Clip_Path;

extern Esvg_Attribute_Animated_Descriptor esvg_attribute_animated_clip_path_descriptor;
void esvg_attribute_clip_path_merge_rel(const Esvg_Attribute_Clip_Path *rel,
		const Esvg_Attribute_Clip_Path *v,
		Esvg_Attribute_Clip_Path *d);
void esvg_attribute_clip_path_unset(Esvg_Attribute_Clip_Path *a, const Esvg_Clip_Path *def);
void esvg_attribute_clip_path_set(Esvg_Attribute_Clip_Path *a, const Esvg_Clip_Path *v,
		const Esvg_Clip_Path *def);
void esvg_attribute_animated_clip_path_merge_rel(const Esvg_Attribute_Animated_Clip_Path *rel,
		const Esvg_Attribute_Animated_Clip_Path *v,
		Esvg_Attribute_Clip_Path *d);
void esvg_attribute_animated_clip_path_merge(const Esvg_Attribute_Animated_Clip_Path *v,
		Esvg_Attribute_Clip_Path *d);
void esvg_attribute_animated_clip_path_set(Esvg_Attribute_Animated_Clip_Path *aa,
	const Esvg_Animated_Clip_Path *v,
	const Esvg_Clip_Path *def,
	Eina_Bool animate);
void esvg_attribute_animated_clip_path_extended_set(Esvg_Attribute_Animated_Clip_Path *aa,
	const Esvg_Animated_Clip_Path *v,
	const Esvg_Clip_Path *def,
	Eina_Bool animate,
	int *set);
void esvg_attribute_animated_clip_path_get(Esvg_Attribute_Animated_Clip_Path *aa,
	Esvg_Animated_Clip_Path *v);
#endif


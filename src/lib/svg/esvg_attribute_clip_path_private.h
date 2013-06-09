#ifndef _ESVG_PRIVATE_ATTRIBUTE_CLIP_PATH_H_
#define _ESVG_PRIVATE_ATTRIBUTE_CLIP_PATH_H_

typedef struct _Egueb_Svg_Attribute_Clip_Path
{
	Egueb_Svg_Clip_Path v;
	Eina_Bool is_set;
} Egueb_Svg_Attribute_Clip_Path;

typedef struct _Egueb_Svg_Attribute_Animated_Clip_Path
{
	Egueb_Svg_Attribute_Clip_Path base;
	Egueb_Svg_Attribute_Clip_Path anim;
	int animated;
} Egueb_Svg_Attribute_Animated_Clip_Path;

extern Egueb_Svg_Attribute_Animated_Descriptor egueb_svg_attribute_animated_clip_path_descriptor;
void egueb_svg_attribute_clip_path_merge_rel(const Egueb_Svg_Attribute_Clip_Path *rel,
		const Egueb_Svg_Attribute_Clip_Path *v,
		Egueb_Svg_Attribute_Clip_Path *d);
void egueb_svg_attribute_clip_path_unset(Egueb_Svg_Attribute_Clip_Path *a, const Egueb_Svg_Clip_Path *def);
void egueb_svg_attribute_clip_path_set(Egueb_Svg_Attribute_Clip_Path *a, const Egueb_Svg_Clip_Path *v,
		const Egueb_Svg_Clip_Path *def);
void egueb_svg_attribute_animated_clip_path_merge_rel(const Egueb_Svg_Attribute_Animated_Clip_Path *rel,
		const Egueb_Svg_Attribute_Animated_Clip_Path *v,
		Egueb_Svg_Attribute_Clip_Path *d);
void egueb_svg_attribute_animated_clip_path_merge(const Egueb_Svg_Attribute_Animated_Clip_Path *v,
		Egueb_Svg_Attribute_Clip_Path *d);
void egueb_svg_attribute_animated_clip_path_set(Egueb_Svg_Attribute_Animated_Clip_Path *aa,
	const Egueb_Svg_Animated_Clip_Path *v,
	const Egueb_Svg_Clip_Path *def,
	Eina_Bool animate);
void egueb_svg_attribute_animated_clip_path_extended_set(Egueb_Svg_Attribute_Animated_Clip_Path *aa,
	const Egueb_Svg_Animated_Clip_Path *v,
	const Egueb_Svg_Clip_Path *def,
	Eina_Bool animate,
	int *set);
void egueb_svg_attribute_animated_clip_path_get(Egueb_Svg_Attribute_Animated_Clip_Path *aa,
	Egueb_Svg_Animated_Clip_Path *v);
#endif


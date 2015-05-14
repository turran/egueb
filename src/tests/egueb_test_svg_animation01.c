#include "Egueb_Svg.h"
#include "Egueb_Smil.h"
#include "check.h"

Egueb_Dom_Node *doc;
Egueb_Dom_Node *svg;
Egueb_Dom_Node *rect;
Egueb_Dom_Node *anim;

void setup(void)
{
	Egueb_Smil_Duration dur;
	Egueb_Dom_Feature *afeature;
	Egueb_Dom_String *value;
	Egueb_Svg_Length length;

	doc = egueb_svg_document_new();
	svg = egueb_svg_element_svg_new();
	egueb_svg_length_set(&length, 256, EGUEB_SVG_LENGTH_UNIT_PX);
	egueb_svg_element_svg_width_set_simple(svg, &length);
	egueb_svg_element_svg_height_set_simple(svg, &length);
	rect = egueb_svg_element_rect_new();
	egueb_svg_element_id_set(rect, egueb_dom_string_new_with_string("rect01"));
	/* create an animation that will animate the rect x */
	anim = egueb_smil_animate_new();
	/* set the href to an nonexistent element */
	egueb_smil_animation_href_set(anim, egueb_dom_string_new_with_string("rect02"));
	/* default clock of 1s */
	dur.type = EGUEB_SMIL_DURATION_TYPE_CLOCK;
	dur.data.clock = EGUEB_SMIL_CLOCK_SECONDS * 1;
	value = egueb_dom_string_new_with_static_string("x");
	egueb_smil_animation_attribute_name_set(anim, value);
	value = egueb_dom_string_new_with_static_string("10px");
	egueb_smil_animate_base_from_set(anim, value); 
	value = egueb_dom_string_new_with_static_string("20px");
	egueb_smil_animate_base_to_set(anim, value); 
	egueb_smil_animation_dur_set(anim, &dur);
	/* add the animation */
	egueb_dom_node_child_append(svg, anim, NULL);
	egueb_dom_node_child_append(svg, rect, NULL);
	egueb_dom_node_child_append(doc, svg, NULL);

	/* tick */
	afeature = egueb_dom_node_feature_get(svg,
			EGUEB_SMIL_FEATURE_ANIMATION_NAME, NULL);
	egueb_smil_feature_animation_tick(afeature);
	egueb_dom_feature_unref(afeature);
	/* process */
	egueb_dom_document_process(doc);
}

void cleanup(void)
{
	egueb_dom_node_unref(doc);
}

START_TEST(egueb_test_svg_animation01_no_id)
{
	Egueb_Svg_Length_Animated ax;

	/* make sure the x is still zero */
	egueb_svg_element_rect_x_get(rect, &ax);
	ck_assert_int_eq(ax.anim.value, 0);
}
END_TEST

START_TEST(egueb_test_svg_animation01_change_id)
{
	Egueb_Svg_Length_Animated ax;
	Egueb_Dom_Feature *afeature;

	/* change the href */
	egueb_smil_animation_href_set(anim, egueb_dom_string_new_with_string("rect01"));

	/* tick */
	afeature = egueb_dom_node_feature_get(svg,
			EGUEB_SMIL_FEATURE_ANIMATION_NAME, NULL);
	egueb_smil_feature_animation_tick(afeature);
	egueb_dom_feature_unref(afeature);
	/* process */
	egueb_dom_document_process(doc);
	/* make sure the x is still zero */
	egueb_svg_element_rect_x_get(rect, &ax);
	ck_assert_int_eq(ax.anim.value, 10);
}
END_TEST

Suite * egueb_test_svg_animation01_suite(void)
{
	Suite *s;
	TCase *tcase;

	s = suite_create("SvgAnimation01");
	tcase = tcase_create("Adding animation with target and no id found");
	tcase_add_checked_fixture(tcase, setup, cleanup);
	tcase_add_test(tcase, egueb_test_svg_animation01_no_id);
	suite_add_tcase(s, tcase);

	tcase = tcase_create("Change the id to a valid id");
	tcase_add_checked_fixture(tcase, setup, cleanup);
	tcase_add_test(tcase, egueb_test_svg_animation01_change_id);
	suite_add_tcase(s, tcase);

	return s;
}

int main(void)
{
	Suite *s;
	SRunner *sr;
	int number_failed;

	/* Create the document */
	egueb_svg_init();

	s = egueb_test_svg_animation01_suite();
	sr = srunner_create(s);

	srunner_run_all(sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);

	egueb_svg_shutdown();

	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}


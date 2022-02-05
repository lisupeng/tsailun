#include "htmlparser_interface.h"

#include "gumbo.h"

// TODO-0 make sure 1.recursive calling and 2.big std::string won't cause stack overflow
// TODO-0 performance optimize
static std::string cleantext(GumboNode* node) {
	if (node->type == GUMBO_NODE_TEXT) {
		return std::string(node->v.text.text);
	}
	else if (node->type == GUMBO_NODE_ELEMENT &&
		node->v.element.tag != GUMBO_TAG_SCRIPT &&
		node->v.element.tag != GUMBO_TAG_STYLE) {
		std::string contents = "";
		GumboVector* children = &node->v.element.children;
		for (unsigned int i = 0; i < children->length; ++i) {
			const std::string text = cleantext((GumboNode*)children->data[i]);
			if (i != 0 && !text.empty()) {
				contents.append(" ");
			}
			contents.append(text);
		}
		return contents;
	}
	else {
		return "";
	}
}

std::string retrieve_text_from_html(QByteArray *html_in_utf8)
{
	GumboOutput* output = gumbo_parse(html_in_utf8->constData());

	std::string ret = cleantext(output->root);

	// TODO-0 optimize, don't copy ret
	gumbo_destroy_output(&kGumboDefaultOptions, output);

	return ret;
}
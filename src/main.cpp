#include <string>
#include <iostream>
#include <fstream>


#define STATIC_STRING_VARIABLE_NAME "__serenity_templater_str"
#define RESULT_VARIABLE_NAME "__serenity_templater_res"
#define MACRO_PREFIX "__SERENITY_TEMPLATER_TEMPLATE_"


namespace {

int returnCode = 0;

void writeText(std::ostream & out, const std::string & text) {
	out << "{static const char " STATIC_STRING_VARIABLE_NAME "[]={";
	out << std::to_string(text[0]);
	for (auto it = ++text.begin(); it != text.end(); it++) { out << ',' << std::to_string(*it); }
	out << "};" RESULT_VARIABLE_NAME ".write(" STATIC_STRING_VARIABLE_NAME ",sizeof(" STATIC_STRING_VARIABLE_NAME "));}";
}

void writeCommand(std::ostream & out, const std::string & command, const std::string & parameters) {
	if ((command == "") && (parameters == "")) return;

	if (command == "")        out << RESULT_VARIABLE_NAME "<<" << parameters << ";"; else  // $(var)
	if (command == "else")    out << "}else{"; else  // $else
	if (command == "end")     out << "}"; else       // $end
	if (parameters == "")     out << RESULT_VARIABLE_NAME "<<" << command << ";"; else  // $var
	if (command == "for")     out << "for(" << parameters << "){"; else        // $for (int i=0; i<n; i++)
	if (command == "foreach") out << "for(auto&&" << parameters << "){"; else  // $foreach(item : collection)
	if (command == "if")      out << "if(" << parameters << "){"; else         // $if (cond)

	std::cerr << "unknown command: $'" << command << "'('" << parameters << "')";
	returnCode = 1;
}

void preprocess(std::istream & in, std::ostream & out, const std::string & templateName) {
	out << "#define " MACRO_PREFIX << templateName << " [&](){std::stringstream " RESULT_VARIABLE_NAME ";";

	enum class State {
		TEXT,                      // skipping to $
		DOLLAR_COMMAND_NAME,       // skipping to '( or non-alphanumeric
		DOLLAR_COMMAND_PARAMETERS  // skipping to matching ')'
	};

	State state = State::TEXT;
	std::string text;
	std::string command;
	std::string parameters;
	int bracketsDepth = 0;

	for (int c_int = in.get(); (c_int != std::char_traits<char>::eof()) && (in.good()); c_int = in.get()) {
		char c = (char)c_int;
		State newState = state;

		if (state == State::TEXT) {
			if (c == '$') { newState = State::DOLLAR_COMMAND_NAME; } else { text += c; }
		} else if (state == State::DOLLAR_COMMAND_NAME) {
			if (isalnum(c) || (c == '_')) {
				command += c;
			} else if (c == '(') {
				newState = State::DOLLAR_COMMAND_PARAMETERS;
				bracketsDepth = 1;
			} else {
				newState = State::TEXT;
				if ((c == '$') && command.empty()) { text += c; } else { in.putback(c); }
			}
		} else if (state == State::DOLLAR_COMMAND_PARAMETERS) {
			if (c == '(') { bracketsDepth++; }
			if (c == ')') { bracketsDepth--; }
			if (bracketsDepth > 0) { parameters += c; } else { newState = State::TEXT; }
		}

		if (newState != state) {
			if (newState == State::TEXT) {
				if (!text.empty()) writeText(out, text);
				if (!command.empty() || !parameters.empty()) writeCommand(out, command, parameters);

				text.clear();
				command.clear();
				parameters.clear();
			}

			state = newState;
		}
	}

	if (!text.empty()) writeText(out, text);
	if (!command.empty() || !parameters.empty()) writeCommand(out, command, parameters);

	out << "return " RESULT_VARIABLE_NAME ".str();}\n";
}

std::string fileNameToTemplateName(const std::string & fileName) {
	auto begin = fileName.find_last_of('/');
	if (begin == std::string::npos) begin = 0;
	auto end = fileName.find('.', begin);
	if (end == std::string::npos) end = fileName.size()-1;
	return fileName.substr(begin+1, end-begin-1);
}

}


int main(int argc, char ** argv) {
	if ((argc < 2) || (std::string(argv[1]) == "-h") || (std::string(argv[1]) == "--help")) {
		printf("Usage:\n  %s output-file.htmltc input-file1.htmlt ... input-fileN.htmlt", argv[0]);
		return 1;
	}

	std::ofstream out(argv[1]);
	out << "#include <serenity/templater.hpp>\n";
	for (int i = 2; i < argc; i++) {
  	std::ifstream in(argv[i], std::ios::in | std::ios::binary);
		preprocess(in, out, fileNameToTemplateName(argv[i]));
	}
	return returnCode;
}

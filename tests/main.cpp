#include <tests/templates.htmltc>


const char * correctAnswer = R"(<html>
<body>
<h1>Hello!</h1>
<h2>This is some text</h2>
<h3>Numbers:</h3>
<ul>
<li>1</li>
<li>2</li>
<li>3</li>
</ul>
<h3>MORE NUMBERS</h3>
<ul>
<li>1.125</li>
<li>2.567</li>
<li>3.874</li>
</ul>
</body>
</html>
)";


TEST_CASE( "preprocess empty" ) {
	std::string res = TEMPLATE(empty);
	CHECK( res == "\n" );
}

TEST_CASE( "preprocess static" ) {
	std::string res = TEMPLATE(static);
	CHECK( res == correctAnswer );
}

TEST_CASE( "preprocess simple string variable" ) {
	std::string title = "Hello!";
	std::string res = TEMPLATE(one_var);
	CHECK( res == correctAnswer );
}

TEST_CASE( "preprocess two string variables without space between them" ) {
	std::string titlePartOne = "Hell";
	std::string titlePartTwo = "o!";
	std::string res = TEMPLATE(two_vars);
	CHECK( res == correctAnswer );
}

TEST_CASE( "preprocess char as int variable" ) {
	char number1 = 1;
	std::string res = TEMPLATE(char_as_int_var);
	CHECK( res == correctAnswer );
}

TEST_CASE( "preprocess int variable" ) {
	int number1 = 1;
	std::string res = TEMPLATE(int_var);
	CHECK( res == correctAnswer );
}

TEST_CASE( "preprocess float variable" ) {
	float number1 = 1.125;
	std::string res = TEMPLATE(float_var);
	CHECK( res == correctAnswer );
}

TEST_CASE( "preprocess std::array and std::vector" ) {
	std::array<unsigned short, 3> ints = {{ 1, 2, 3 }};
	std::vector<double> floats = {{ 1.125, 2.567, 3.874 }};
	std::string res = TEMPLATE(array_vector);
	CHECK( res == correctAnswer );
}

TEST_CASE( "preprocess formatted floats" ) {
	std::array<unsigned short, 3> ints = {{ 1, 2, 3 }};
	std::vector<double> floats = {{ 1.1254444, 2.5673333, 3.8742222 }};
	std::string res = TEMPLATE(formatted_floats);
	CHECK( res == correctAnswer );
}


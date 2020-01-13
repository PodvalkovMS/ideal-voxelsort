#include "stdafx.h"
#include "vtk_vis.h"

typedef std::vector<std::vector<double> > my_vector_of_vectors_t;

using namespace std;
using namespace boost::filesystem;
using namespace boost::program_options;
using namespace nanoflann;

string ChangeToObj(const string& pathToFileInv) {
	auto pos = pathToFileInv.find_first_of(".");
	string invFName;
	if (pos != string::npos && pos != 0)
	{
		invFName = pathToFileInv.substr(0, pos);
	}
	else
	{
		cerr << "No extension in input filename? : " << pathToFileInv << endl;
	}
	invFName += ".obj";

	return invFName;
}

auto parse_cli_args(int argc, char** argv)
{
	options_description desc{ "Program options of number closest vectors." };
	desc.add_options()
		("help,h", "-i input.obj -o output.ply")
		("inputd,d", value<string>(), "Path to files directory ")
		("inputf,f", value<string>(), " Path to file ")
		("number,n", value<int>()->default_value(10), " number of closest object")

		/*("max-angle", value<double>()->default_value(20), "Max accepted angle for the region growing algorithm (see reference).")
		("max-distance", value<double>()->default_value(0.4), "Max distance to plane for the region growing algorithm (see reference).")
		("min-regsize",value<long>()->default_value(3), "A minimum size of region for region growing algorithm. Number of points (see reference).")
		("min-plane-area", value<double>()->default_value(1), "A minimum area of the plane to detection.")
		("clean,c", bool_switch(), "Make some mesh preprocessing: merge duplicate points, merge duplicate polygons and try to orient.")
		*/;

	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);

	return tuple<variables_map, options_description>(vm, desc);
}

bool validate_args(const variables_map& args, const options_description& desc)
{
	/*if (args.count("input") !=3)
	{
		cout << "Mssing required argument 'input' or it occured multiple times." << endl;
		cout << desc << endl;
		return false;
	}*/

	path input_direct{ args["inputd"].as<string>() };
	path input_file{ args["inputf"].as<string>() };

	if (status(input_direct).type() != file_type::directory_file)
	{
		cerr << "The directory " << input_file << " is not directory or does not exist." << endl;
		return false;
	}

	if (status(input_file).type() != file_type::regular_file)
	{
		cerr << "The regular " << input_file << " is not regular or does not exist." << endl;
		return false;
	}

	int number{ args["number"].as<int>() };

	if (number <= 0)
	{
		cerr << "Invalid value for: " << number << " Accepted values are in [1; obj in derictory]" << endl;
		return false;
	}

	return true;
}

vector<double> FileToVector(std::ifstream& path) {
	vector<double> vectorIscomNumbers;
	setlocale(LC_NUMERIC, "C");
	if (path.is_open())
	{
		std::string s, s1 = "";
		getline(path, s);
		getline(path, s);
		for (auto x : s) {
			if (x != ' ') {
				s1 += x;
			}
			else
			{
				vectorIscomNumbers.push_back(stod(s1));
				s1 = "";
			}
		}
	}
	else
	{
		cerr << "Cannot read search file " << endl;
	}

	return vectorIscomNumbers;
}

vector<path> AllFilesInDirectory(path& DirectName) {
	vector<path> v;
	for (auto&& x : recursive_directory_iterator(DirectName)) {
		if (x.path().extension() != ".inv") {
			continue;
		}
		else {
			v.push_back(x.path());
		}
	}
	if (v.size() == 0)
	{
		cerr << "Cannot find file to read in this directory " << DirectName.string() << endl;
	}

	return v;
}

int main(int argc, char** argv)
{
	variables_map args;

	try
	{
		auto res_tuple = parse_cli_args(argc, argv);
		args = get<0>(res_tuple);
		auto desc = get<1>(res_tuple);

		if (args.count("help"))
		{
			cout << desc << endl;
			return 0;
		}

		bool is_valid{ validate_args(args, desc) };

		if (!is_valid)
		{
			return 1;
		}
	}
	catch (error & err)
	{
		cerr << err.what() << endl;
		return 1;
	}
	/*
		if (argc != 3)
		{
			cout << "Usage: ZernikeMoments " <<
				"directname " <<
				"filename search" << endl;
			return 0;
		}
	*/
	/*std::string argv1;
	std::cin >>  argv1;
	std::string argv2;
	std::cin >> argv2; */

	path DirectName(args["inputd"].as<string>() /**/);
	vector<path> v;
	v = AllFilesInDirectory(DirectName);
	/*	for (auto&& x : boost::filesystem::directory_iterator(DirectName)) {
			auto k = x.path();
			std::string z = k.string();
			if (z[z.size() - 1] != 'v') {
				continue;
			}
			else {
				v.push_back(x.path());
			}
		}
		if (v.size() == 0)
		{
			std::cerr << "Cannot find file to read in this directory " << argv[1] << std::endl;
		}
	*/
	path pat(args["inputf"].as<string>() /**/);
	std::ifstream ifs(pat.string());
	vector<double> NumbersIscom;
	NumbersIscom = FileToVector(ifs);
	if (NumbersIscom.size() == 0)
	{
		cerr << "Cannot read file " << args["inputf"].as<string>() << endl;
	}

	my_vector_of_vectors_t AllVectors;
	vector <string> PathToVector;

	for (size_t i = 0; i < v.size() - 1; ++i) {
		if (args["inputf"].as<string>() /**/ != v[i].string()) {
			string invFName = v[i].string();

			std::ifstream in(invFName);

			AllVectors.push_back(FileToVector(in));
			PathToVector.push_back(v[i].string());
		}
	}

	size_t dim;
	dim = AllVectors[0].size();
	typedef KDTreeVectorOfVectorsAdaptor< my_vector_of_vectors_t, double >  my_kd_tree_t;
	my_kd_tree_t   mat_index(dim /*dim*/, AllVectors, 10 /* max leaf */);
	mat_index.index->buildIndex();
	// do a knn search
	const size_t num_results = args["number"].as<int>();
	vector<size_t>   ret_indexes(num_results);
	vector<double> out_dists_sqr(num_results);

	KNNResultSet<double> resultSet(num_results);

	resultSet.init(&ret_indexes[0], &out_dists_sqr[0]);
	mat_index.index->findNeighbors(resultSet, &NumbersIscom[0], SearchParams(10));

	std::ofstream file(args["inputd"].as<string>() + "\\Answer.txt");
	file << "For " << endl;
	file << args["inputf"].as<string>()/**/ << endl;
	file << "Ten closest vector is" << endl;

	for (size_t i = 0; i < num_results; i++) {
		file << i + 1 << PathToVector[ret_indexes[i]] << "  distance=" << out_dists_sqr[i] << endl;
	}
	file.close();

	/*auto pos = args["inputf"].as<string>().find_first_of(".");
	string invFName;
	if (pos != string::npos && pos != 0)
	{
		invFName = args["inputf"].as<string>().substr(0, pos);
	}
	else
	{
		cerr << "No extension in input filename? : " << args["inputf"].as<string>() << endl;
	}
	invFName += ".obj";

	path pathfile(invFName);
	const char* ch = new char[invFName.length()];
	ch= invFName.c_str();

	show_ply(ch);*/
	vector <string> PathToObj;
	PathToObj.push_back(ChangeToObj(args["inputf"].as<string>()));
	for (size_t i = 0; i < num_results; i++) {
		PathToObj.push_back(ChangeToObj(PathToVector[ret_indexes[i]]));
	}

	show_ply(PathToObj);
	return 0;
}
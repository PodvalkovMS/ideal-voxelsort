#include "stdafx.h"
#include "vtk_vis.h"
#include "make_tree.h"

typedef std::vector<std::vector<double> > my_vector_of_vectors_t;

using namespace std;
using namespace boost::filesystem;
using namespace boost::program_options;
using namespace nanoflann;

path change_to_obj(const path& path_to_file_inv,  const string& obj_extansion, int& eroor )  { ///
	path pos = path_to_file_inv.stem(), pos2=path_to_file_inv.branch_path();
	string file_name_ext = pos.string() + obj_extansion;
	path path_file_name(file_name_ext);
	pos2 /=path_file_name;
	std::ifstream p_f_n;
	p_f_n.open(pos2.string());
	
	if (!p_f_n.is_open()) {
		cerr << "No extension in input filename? : " << path_to_file_inv << endl;
		eroor = 1;
	}
	else {
		eroor = 0;
		}
	p_f_n.close();
	return pos2;
}

auto parse_cli_args(int argc, char** argv)
{
	options_description desc{ "Program options of number closest vectors." };
	desc.add_options()
		("help,h", "-i input.obj -o output.ply")
		("inputd,d", value<string>(), "Path to files directory ")
		("inputf,f", value<string>(), " Path to file ")
		("number,n", value<int>()->default_value(10), " number of closest object");

	variables_map vm;
	store(parse_command_line(argc, argv, desc), vm);
	notify(vm);

	return tuple<variables_map, options_description>(vm, desc);
}

bool validate_args(const variables_map& args, const options_description& desc)
{
	
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

int check_flag(const  std::ifstream& ifs){
	int error = 0;
	if (!ifs.goodbit) {
		if (!ifs.eofbit) {
			cout << "End-Of-File reached while performing an extracting operation on an input stream." << endl;
			error = 1;
		}
		if (!ifs.failbit) {
			cout << "The last input operation failed because of an error related to the internal logic of the operation itself." << endl;
			error = 1;
			if (!ifs.badbit) {
				cout << "Error due to the failure of an input / output operation on the stream buffer." << endl;
				error = 1;
			}
		}
	}
	return error;
}

vector<double> file_to_vector(path& path, const string& extantion, int size_of_vector, int error) {
	vector<double> vector_iscom_numbers, carbige;
	double trash;
	error = 0;
	int i=0;
	std::ifstream ifs(path.string());
	if (path.extension() == extantion) {
		if (ifs.is_open())
		{
			ifs >> size_of_vector;
			error=check_flag(ifs);
			if (error == 1)
			{
				i++;
			}
			while (!ifs.eof())
			{
				ifs >> trash;
				if (ifs.eof())
				{
					break;
				}
				error = check_flag(ifs);
				if (error == 1)
				{
					i++;
				}
				vector_iscom_numbers.push_back(trash);
				

			} 
			
		}
		else
		{
			cerr << "Cannot open file " << endl;
			error = 1;
		}
	}
	else
	{
		cout << "Error expected .inv" << endl;
		error = 1;
	}
	if ((i!=0) || (error==1))
	{
		vector_iscom_numbers = carbige;
	}
	return vector_iscom_numbers;
}

vector<path> all_files_in_directory(path& direct_name, const string& extantion, int error) {
	vector<path> v;
	
	for (auto&& x : recursive_directory_iterator(direct_name)) {
		if (x.path().extension() != extantion) {
			continue;
		}
		else {
			v.push_back(x.path());
		}
	}
	if (v.size() == 0)
	{
		cerr << "Cannot find file to read in this directory " << direct_name.string() << endl;
		error = 1;
	}
	error = 0;
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


	vector<path> v;	
	string extension = ".inv";
	int errorr=0, size_of_iscom_vector=0;
	path direct_name(args["inputd"].as<string>() /**/);
	path file_name(args["inputf"].as<string>() /**/);
	
	v = all_files_in_directory(direct_name, extension, errorr);
	if (errorr == 1) {
		return 1;
	}

	vector<double> numbers_iscom;
	numbers_iscom = file_to_vector(file_name, extension, size_of_iscom_vector, errorr);
	
	if (numbers_iscom.size() == 0)
	{
		cerr << "File is empty " << file_name << endl;
		return 1;
	}
	if (errorr == 1) {
		return 1;
	}
	
	
	my_vector_of_vectors_t all_vectors;
	vector <path> path_to_vector;

	for (size_t i = 0; i < v.size() - 1; ++i) {
		if (file_name.string()  != v[i].string()) {
			vector<double> trash;
			int sizes_of_vectors=0;
			trash = file_to_vector(v[i], extension, sizes_of_vectors, errorr);
			if ((size_of_iscom_vector==sizes_of_vectors)&& (errorr==0)) {
			all_vectors.push_back(trash);
			path_to_vector.push_back(v[i]);
			}		
		}
	}

	if (path_to_vector.size() == 0) {

		cout << "Not find suitable file in" << direct_name.string() << endl;
		return 1;

	}
	
	const size_t num_results = args["number"].as<int>();
	size_t dim = all_vectors[0].size();
	
	std::vector<size_t>   ret_indexes(num_results);
	std::vector<double>  out_dists_sqr(num_results);
	int max_leaf = 10;

	Tot_tree gor{ all_vectors, dim, max_leaf };
	gor.find_near(numbers_iscom, ret_indexes, out_dists_sqr, num_results);


	path answer("Answer.txt"), new_direct_name_for_answer;
	new_direct_name_for_answer = direct_name;
	new_direct_name_for_answer /= answer;
	
	std::ofstream file(new_direct_name_for_answer.string()); 
	
	file << file_name.string() << endl;
	file << num_results << endl;

	for (size_t i = 0; i < num_results; i++) {
		file << path_to_vector[ret_indexes[i]] << "  distance=" << out_dists_sqr[i] << endl;
	}
	file.close();
	path garbige;
	string obj = ".obj";
	vector <path> path_to_obj;
	int mistake;
	garbige = change_to_obj(file_name, obj, mistake);
	if (mistake == 1) {
		return 1;
	}
	path_to_obj.push_back(garbige);
	for (size_t i = 0; i < num_results; i++) {
		garbige = change_to_obj(path_to_vector[ret_indexes[i]], obj, mistake);
		if (mistake == 0) {
		path_to_obj.push_back(garbige);
		}	
	}

	vtk_viz::show_ply(path_to_obj);
	return 0;
}
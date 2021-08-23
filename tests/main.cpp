#define USE_UNSTABLE_GEOS_CPP_API
#include <global.h>

int test(int argc, char **argv);
int test_parallel(int argc, char **argv);

int main(int argc, char **argv)
{
    test(argc, argv);
    //test_parallel(argc, argv);
    return 0;
}

int test(int argc, char **argv)
{
    spdlog::set_pattern(" [%H:%M:%S.%e] %v");

#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
#else
    spdlog::set_level(spdlog::level::info); // Set global log level to info
#endif

    double time_total, time_parse, time_index, time_query;
    time_total =  time_parse = time_index = time_query = 0.0;
    ulong result = 0;

    std::chrono::duration<double> t_diff_temp;

    auto t_begin = std::chrono::steady_clock::now();

    const std::string file_path_1 = argv[1];
    const std::string file_path_2 = argv[2];

    spdlog::info("FILE PATH1 {} :: FILE PATH2{}", file_path_1, file_path_2);

    std::list<geos::geom::Envelope *> *list_envs_1 = new std::list<geos::geom::Envelope *>();
    std::list<geos::geom::Envelope *> *list_envs_2 = new std::list<geos::geom::Envelope *>();

    gsj::Reader rd;

    rd.Read_Envs_from_file(file_path_1, list_envs_1);
    rd.Read_Envs_from_file(file_path_2, list_envs_2);

    spdlog::debug("L1 {}  L2 {}", list_envs_1->size(), list_envs_2->size());
	
    auto t_parse_end = std::chrono::steady_clock::now();

    /************************Build index************************************************************************/
    geos::index::strtree::STRtree index_for_layer_1;
    for (std::list<geos::geom::Envelope *>::iterator itr = list_envs_1->begin(); itr != list_envs_1->end(); ++itr)
    {
	    geos::geom::Envelope * temp_env = *itr;

	    index_for_layer_1.insert(temp_env, temp_env);
    }

    auto t_index_end = std::chrono::steady_clock::now();
    spdlog::debug("Index built");

    /************************Build index end************************************************************************/

    uint counter = 0;
    for (std::list<geos::geom::Envelope *>::iterator itr = list_envs_2->begin(); itr != list_envs_2->end(); ++itr)
    {
	    geos::geom::Envelope * temp_env = *itr;
	    std::vector<void *> results;

	    index_for_layer_1.query(temp_env, results);
	    result += results.size();

	    spdlog::debug("{} {}", counter++, results.size());

	    results.clear();
            results.shrink_to_fit();
    }

    auto t_query_end = std::chrono::steady_clock::now();

    t_diff_temp = t_query_end - t_begin;
    time_total = t_diff_temp.count();

    t_diff_temp = t_parse_end - t_begin;
    time_parse = t_diff_temp.count();

    t_diff_temp = t_index_end - t_parse_end;
    time_index = t_diff_temp.count();

    t_diff_temp = t_query_end - t_index_end;
    time_query = t_diff_temp.count();

    
    spdlog::info("Results: {}; total time {}; parse {}; index::query {} {}, index + query {}", result, time_total, 
		    time_parse, time_index, time_query, time_index+time_query);

    return 0;
}

int test_parallel(int argc, char **argv)
{
    spdlog::set_pattern(" [%H:%M:%S.%e] %v");

#ifdef DEBUG
    spdlog::set_level(spdlog::level::debug); // Set global log level to debug
#else
    spdlog::set_level(spdlog::level::info); // Set global log level to info
#endif

    double time_total, time_parse, time_index, time_query;
    time_total =  time_parse = time_index = time_query = 0.0;
    ulong result = 0;

    std::chrono::duration<double> t_diff_temp;

    auto t_begin = std::chrono::steady_clock::now();

    const std::string file_path_1 = argv[1];
    const std::string file_path_2 = argv[2];

    spdlog::info("FILE PATH1 {} :: FILE PATH2{}", file_path_1, file_path_2);

    std::list<geos::geom::Envelope *> *list_envs_1 = new std::list<geos::geom::Envelope *>();
    std::list<geos::geom::Envelope *> *list_envs_2 = new std::list<geos::geom::Envelope *>();

    gsj::Reader rd;

    rd.Read_Envs_from_file(file_path_1, list_envs_1);
    rd.Read_Envs_from_file(file_path_2, list_envs_2);

    spdlog::debug("L1 {}  L2 {}", list_envs_1->size(), list_envs_2->size());
	
    auto t_parse_end = std::chrono::steady_clock::now();

    /************************Build index************************************************************************/
    geos::index::strtree::STRtree index_for_layer_1;
    for (std::list<geos::geom::Envelope *>::iterator itr = list_envs_1->begin(); itr != list_envs_1->end(); ++itr)
    {
	    geos::geom::Envelope * temp_env = *itr;

	    index_for_layer_1.insert(temp_env, temp_env);
    }

    auto t_index_end = std::chrono::steady_clock::now();

    /************************Build index end************************************************************************/

    for (std::list<geos::geom::Envelope *>::iterator itr = list_envs_2->begin(); itr != list_envs_2->end(); ++itr)
    {
	    geos::geom::Envelope * temp_env = *itr;
	    std::vector<void *> results;

	    index_for_layer_1.query(temp_env, results);
	    result += results.size();
    }

    auto t_query_end = std::chrono::steady_clock::now();

    t_diff_temp = t_query_end - t_begin;
    time_total = t_diff_temp.count();

    t_diff_temp = t_parse_end - t_begin;
    time_parse = t_diff_temp.count();

    t_diff_temp = t_index_end - t_parse_end;
    time_index = t_diff_temp.count();

    t_diff_temp = t_query_end - t_index_end;
    time_query = t_diff_temp.count();

    
    spdlog::info("Results: {}; total time {}; parse {}; index::query {} {}, index + query {}", result, time_total, 
		    time_parse, time_index, time_query, time_index+time_query);

    return 0;
}


#include <benchmark/benchmark.h>

#include <fstream>
#include <string>

#include "../src/parser.hpp"
#include <Compiler/AST/Parser.hpp>

std::string readFile(const std::string& filename)
{
    std::ifstream stream(filename);
    std::string code((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    return code;
}

constexpr int simple = 0, medium = 1, big = 2;

static void BM_Parse(benchmark::State& state)
{
    const long selection = state.range(0);
    const std::string filename = (selection == simple) ? "new/simple.ark" : ((selection == medium) ? "new/medium.ark" : "new/big.ark");
    const std::string code = readFile(filename);
    long linesCount = 0;
    for (std::size_t i = 0, end = code.size(); i < end; ++i) if (code[i] == '\n') ++linesCount;

    long long nodes = 0;
    long long lines = 0;

    for (auto _ : state)
    {
        Parser parser(code, false);
        parser.parse();

        nodes += parser.ast().list().size();
        lines += linesCount;
    }

    state.counters["nodesRate"] = benchmark::Counter(nodes, benchmark::Counter::kIsRate);
    state.counters["nodesAvg"] = benchmark::Counter(nodes, benchmark::Counter::kAvgThreads);
    state.counters["uselessLines/sec"] = benchmark::Counter(lines, benchmark::Counter::kIsRate);
}

BENCHMARK(BM_Parse)->Name("New parser - Simple - 39 nodes")->Arg(simple)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Parse)->Name("New parser - Medium - 83 nodes")->Arg(medium)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_Parse)->Name("New parser - Big - 665 nodes")->Arg(big)->Unit(benchmark::kMillisecond);

static void BM_LegacyParse(benchmark::State& state)
{
    const long selection = state.range(0);
    const std::string filename = (selection == simple) ? "legacy/simple.ark" : ((selection == medium) ? "legacy/medium.ark" : "legacy/big.ark");
    const std::string code = readFile(filename);
    long linesCount = 0;
    for (std::size_t i = 0, end = code.size(); i < end; ++i) if (code[i] == '\n') ++linesCount;

    long long nodes = 0;
    long long lines = 0;

    for (auto _ : state)
    {
        Ark::internal::Parser parser(false, 0, {});
        parser.feed(code);

        nodes += parser.ast().constList().size();
        lines += linesCount;
    }

    state.counters["nodesRate"] = benchmark::Counter(nodes, benchmark::Counter::kIsRate);
    state.counters["nodesAvg"] = benchmark::Counter(nodes, benchmark::Counter::kAvgThreads);
    state.counters["uselessLines/sec"] = benchmark::Counter(lines, benchmark::Counter::kIsRate);
}

BENCHMARK(BM_LegacyParse)->Name("Legacy parser - Simple - 39 nodes")->Arg(simple)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_LegacyParse)->Name("Legacy parser - Medium - 83 nodes")->Arg(medium)->Unit(benchmark::kMillisecond);
BENCHMARK(BM_LegacyParse)->Name("Legacy parser - Big - 665 nodes")->Arg(big)->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();
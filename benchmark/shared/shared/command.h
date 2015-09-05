#ifndef BENCHMARK_SHARED_COMMAND_H
#define BENCHMARK_SHARED_COMMAND_H

namespace Benchmark
{
	enum class Command
	{
		BenchmarkSelect,			// select specified benchmark
		BenchmarkReady,				// selected benchmark is ready for execution
		BenchmarkNotReady,			// selected benchmark is not ready for execution
		BenchmarkDone,				// current benchmark done

		SomeData,					// some data
		SomeDataReceived,			// some data has received

		ByeBye
	};

	enum BenchmarkType
	{
		BenchmarkNone = 0,

		/* 100 consecutive connections, 10MB data */
		Benchmark0001,				// 1 thread per connection
		Benchmark0002,				// 1 thread
		Benchmark0003,				// 10 threads
		Benchmark0004,				// 100 threads
		Benchmark0005,				// selecting by server

		/* 100 simultaneous connections, 10MB data */
		Benchmark0006,				// 1 thread per connection
		Benchmark0007,				// 1 thread
		Benchmark0008,				// 10 threads
		Benchmark0009,				// 100 threads
		Benchmark0010,				// selecting by server

		BenchmarkMax
	};
}

#endif // BENCHMARK_SHARED_COMMAND_H


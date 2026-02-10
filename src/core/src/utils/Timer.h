#pragma once

namespace Utils
{
	class Timer
	{
	public:
		explicit Timer(const char* name);

		double GetElapsed() const;

		std::string ToString() const;

		void Reset();

	private:
		std::string mName;
		double initTime{};
	};

	inline Timer::Timer(const char* name = "Timer")
	{
		mName = std::string(name);
		Reset();
	}


	inline double Timer::GetElapsed() const
	{
		return glfwGetTime() - initTime;
	}

	inline std::string Timer::ToString() const
	{
		return std::string(mName + ": " + std::to_string(GetElapsed()) + "s");
	}

	inline void Timer::Reset()
	{
		initTime = glfwGetTime();
	}
};
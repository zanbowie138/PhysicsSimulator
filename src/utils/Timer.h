#pragma once

#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Utils
{
	class Timer
	{
	public:
		Timer();
		Timer(const char* name);

		double GetElapsed() const;

		std::string ToString() const;

		void Reset();

	private:
		std::string myName;
		double initTime;
	};

	inline Timer::Timer()
	{
		myName = "Timer";
		Reset();
	}

	inline Timer::Timer(const char* name)
	{
		myName = std::string(name);
		Reset();
	}


	inline double Timer::GetElapsed() const
	{
		return glfwGetTime() - initTime;
	}

	inline std::string Timer::ToString() const
	{
		return std::string(myName + ": " + std::to_string(GetElapsed()) + "s");
	}

	inline void Timer::Reset()
	{
		initTime = glfwGetTime();
	}
};
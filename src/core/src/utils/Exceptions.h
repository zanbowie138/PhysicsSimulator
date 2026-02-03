#pragma once
#include <stdexcept>
#include <string>

class EngineException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

class ECSException : public EngineException {
    using EngineException::EngineException;
};

class ResourceException : public EngineException {
    using EngineException::EngineException;
};

class RenderException : public EngineException {
    using EngineException::EngineException;
};

class PhysicsException : public EngineException {
    using EngineException::EngineException;
};

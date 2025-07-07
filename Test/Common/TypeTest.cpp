#include "Logger.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <memory>
#include <string>
using namespace MEngine;
class IAnimal
{
  public:
    virtual ~IAnimal() = default;
    virtual void Speak() const = 0;
};
class Animal : public IAnimal
{
  public:
    virtual ~Animal() = default;
    virtual void Speak() const override
    {
        LogInfo("Animal speaks");
    }
};
class Dog : public Animal
{
  public:
    virtual ~Dog() = default;
    virtual void Speak() const override
    {
        LogInfo("Dog barks");
    }
};
class Cat : public Animal
{
  public:
    virtual ~Cat() = default;
    virtual void Speak() const override
    {
        LogInfo("Cat meows");
    }
};

TEST(TypeTest, typeid)
{
    std::shared_ptr<IAnimal> animal = std::make_shared<Dog>();
    GTEST_LOG_(INFO) << "Animal type: " << typeid(animal).name();
}

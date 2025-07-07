#include "gtest/gtest.h"
#include <entt/core/any.hpp>
#include <entt/meta/factory.hpp>
#include <entt/meta/meta.hpp>
#include <gtest/gtest.h>
#include <memory>
using namespace entt::literals;
class Person
{
  private:
    std::string mName;
    int mAge;

  public:
    Person(const std::string &name, int age) : mName(name), mAge(age)
    {
    }
    virtual ~Person() = default;
    inline const std::string &GetName() const
    {
        return mName;
    }

    inline int GetAge() const
    {
        return mAge;
    }
    inline void SetName(const std::string &name)
    {
        mName = name;
    }
    inline void SetAge(int age)
    {
        mAge = age;
    }
};
class Student : public Person
{
  private:
    std::string mSchool;

  public:
    Student(const std::string &name, int age, const std::string &school) : Person(name, age), mSchool(school)
    {
    }
    virtual ~Student() = default;
    inline const std::string &GetSchool() const
    {
        return mSchool;
    }
    inline void SetSchool(const std::string &school)
    {
        mSchool = school;
    }
};

class ReflTest : public ::testing::Test
{
  protected:
    void SetUp() override
    {
        entt::meta_factory<Person>()
            .type("Person"_hs)
            .data<&Person::SetAge, &Person::GetAge>("Age"_hs)
            .data<&Person::SetName, &Person::GetName>("Name"_hs);
        entt::meta_factory<Student>()
            .type("Student"_hs)
            .base<Person>()
            .data<&Student::SetSchool, &Student::GetSchool>("School"_hs);
    }
    void TearDown() override
    {
    }
};

TEST_F(ReflTest, TestPerson)
{
    // auto student = std::make_shared<Student>("John Doe", 20, "MIT");
    Student student("John Doe", 20, "MIT");
    auto studentAny = entt::forward_as_meta(student);
    auto studentType = entt::resolve<Student>();
    auto schoolField = studentType.data("School"_hs);
    GTEST_LOG_(INFO) << "Student School: " << schoolField.get(studentAny).cast<std::string>();
    schoolField.set(studentAny, std::string("Harvard"));
    GTEST_LOG_(INFO) << "Student School after set: " << schoolField.get(studentAny).cast<std::string>();
}
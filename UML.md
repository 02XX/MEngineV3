### ✅ UML 类图关系总览表

| 类型   | 符号   | 意义描述                         | 生命周期关系             | 示例关系                            |
|--------|--------|----------------------------------|--------------------------|-------------------------------------|
| 扩展   | `<\|--` | 表示继承关系，类在层次结构中的特化 | 子类依赖父类存在         | `Dog` 是 `Animal`                   |
| 实现   | `<\|..` | 表示类实现接口                    | 实现接口的契约           | `Airplane` 实现 `IFlyable`          |
| 构成   | `*--`  | 表示强组合，没有整体就没有部分     | 部件随整体销毁           | `Engine` 属于 `Car`                 |
| 聚合   | `o--`  | 表示弱组合，部分可独立于整体存在   | 部件可独立存在           | `Player` 属于 `Team`                |
| 依赖   | `-->`  | 表示使用关系（如参数、局部变量）   | 使用即依赖               | `OrderService` 使用 `Order`         |
| 弱依赖 | `..>`  | 表示更弱的使用关系（如配置、反射） | 可选、反射、配置注入等   | `ConfigLoader` 可能使用 `Logger`    |


@startuml
class Animal
class Dog
Dog --|> Animal : extends

interface IFlyable
class Airplane
Airplane ..|> IFlyable : implements

class Car
class Engine
Car *-- Engine : composition

class Team
class Player
Team o-- Player : aggregation

class Order
class OrderService
OrderService --> Order : dependency

class Logger
class ConfigLoader
ConfigLoader ..> Logger : weak dependency

@enduml


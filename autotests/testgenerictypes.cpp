/*
  This file is part of the KTextTemplate library

  SPDX-FileCopyrightText: 2010 Michael Jansen <kde@michael-jansen.biz>
  SPDX-FileCopyrightText: 2010 Stephen Kelly <steveire@gmail.com>

  SPDX-License-Identifier: LGPL-2.1-or-later

*/

#include "engine.h"
#include "ktexttemplate_paths.h"
#include "metatype.h"
#include "template.h"
#include "test_macros.h"

#include <QMetaType>
#include <QQueue>
#include <QStack>
#include <QTest>
#include <QVariant>
#include <QVariantHash>

#include <deque>
#include <string>

#include <memory>

Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(ThreeArray)

Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE(QtUnorderedMap)

Q_DECLARE_SMART_POINTER_METATYPE(std::shared_ptr)

Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE(std::deque)

class TestGenericTypes : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase();

    void testGenericClassType();
    void testSequentialContainer_Variant();
    void testAssociativeContainer_Variant();
    void testSequentialContainer_Type();
    void testAssociativeContainer_Type();
    void testSharedPointer();
    void testThirdPartySharedPointer();
    void testNestedContainers();

    void testCustomQObjectDerived();

    void propertyMacroTypes();

    void testUnregistered();
    void testPointerNonQObject();
    void testQGadget();
    void testGadgetMetaType();

}; // class TestGenericTypes

class Person
{
public:
    Person()
        : age(0)
    {
    }
    Person(std::string _name, int _age)
        : name(_name)
        , age(_age)
    {
        static auto _uid = 0;
        uid = ++_uid;
    }

    bool operator==(const Person &other) const
    {
        return uid == other.uid;
    }

    std::string name;
    int age;
    int uid;
};

QDebug operator<<(QDebug debug, const Person &person)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "Person(" << person.uid << ", name:" << person.name << ", age: " << person.age << ')';
    return debug;
}

class PersonGadget
{
    Q_GADGET
    Q_PROPERTY(QString name MEMBER m_name)
public:
    QString m_name;
    int m_age = 42;
};

int qHash(const Person &p)
{
    return p.uid;
}

Q_DECLARE_METATYPE(Person)
Q_DECLARE_METATYPE(PersonGadget)

KTEXTTEMPLATE_BEGIN_LOOKUP(Person)
if (property == QStringLiteral("name"))
    return QString::fromStdString(object.name);
if (property == QStringLiteral("age"))
    return object.age;
KTEXTTEMPLATE_END_LOOKUP

KTEXTTEMPLATE_BEGIN_LOOKUP(PersonGadget)
if (property == QStringLiteral("age"))
    return object.m_age;
KTEXTTEMPLATE_END_LOOKUP

class PersonObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(int age READ age)
public:
    PersonObject(const QString &name, int age, QObject *parent = {})
        : QObject(parent)
        , m_name(name)
        , m_age(age)
    {
    }

    QString name() const
    {
        return m_name;
    }
    int age() const
    {
        return m_age;
    }

private:
    const QString m_name;
    const int m_age; // Yeah, you wish...
};

void TestGenericTypes::initTestCase()
{
    // Register the handler for our custom type
    KTextTemplate::registerMetaType<Person>();
    KTextTemplate::registerMetaType<PersonGadget>();
}

void TestGenericTypes::testGenericClassType()
{
    KTextTemplate::Engine engine;

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    auto t1 = engine.newTemplate(QStringLiteral("Person: \nName: {{p.name}}\nAge: {{p.age}}\nUnknown: {{p.unknown}}"), QStringLiteral("template1"));

    // Check it
    QVariantHash h;
    Person p("Grant Lee", 2);
    h.insert(QStringLiteral("p"), QVariant::fromValue(p));
    KTextTemplate::Context c(h);
    QCOMPARE(t1->render(&c), QStringLiteral("Person: \nName: Grant Lee\nAge: 2\nUnknown: "));
}

static QMap<int, Person> getPeople()
{
    QMap<int, Person> people;
    people.insert(23, Person("Claire", 23));
    people.insert(32, Person("Grant", 32));
    people.insert(50, Person("Alan", 50));
    return people;
}

template<typename SequentialContainer>
void insertPeopleVariants(KTextTemplate::Context &c)
{
    auto people = getPeople();
    auto it = people.constBegin();
    const auto end = people.constEnd();
    SequentialContainer container;
    for (; it != end; ++it)
        container.push_back(QVariant::fromValue(it.value()));
    c.insert(QStringLiteral("people"), QVariant::fromValue(container));
}

template<typename AssociativeContainer>
void insertAssociatedPeopleVariants(KTextTemplate::Context &c)
{
    auto people = getPeople();
    auto it = people.constBegin();
    const auto end = people.constEnd();
    AssociativeContainer container;
    for (; it != end; ++it)
        container.insert(QString::number(it.key()), QVariant::fromValue(it.value()));
    c.insert(QStringLiteral("people"), QVariant::fromValue(container));
}

template<>
void insertPeopleVariants<QMap<QString, QVariant>>(KTextTemplate::Context &c)
{
    insertAssociatedPeopleVariants<QMap<QString, QVariant>>(c);
}

template<>
void insertPeopleVariants<QHash<QString, QVariant>>(KTextTemplate::Context &c)
{
    insertAssociatedPeopleVariants<QHash<QString, QVariant>>(c);
}

template<typename Container>
void testSequentialIteration(KTextTemplate::Context &c)
{
    KTextTemplate::Engine engine;

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    {
        KTextTemplate::Template t1 =
            engine.newTemplate(QStringLiteral("{% for person in people %}{{ person.name }},{% endfor %}"), QStringLiteral("people_template"));
        QCOMPARE(t1->render(&c), QStringLiteral("Claire,Grant,Alan,"));
    }
}

template<typename Container>
void testSequentialIndexing(KTextTemplate::Context &c)
{
    KTextTemplate::Engine engine;

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    {
        KTextTemplate::Template t1 =
            engine.newTemplate(QStringLiteral("{{ people.0.name }},{{ people.1.name }},{{ people.2.name }},"), QStringLiteral("people_template"));
        QCOMPARE(t1->render(&c), QStringLiteral("Claire,Grant,Alan,"));
    }
}

template<typename Container>
struct SequentialContainerTester {
    static void iteration(KTextTemplate::Context &c)
    {
        testSequentialIteration<Container>(c);
    }

    static void indexing(KTextTemplate::Context &c)
    {
        testSequentialIndexing<Container>(c);
    }
};

template<typename T>
struct SequentialContainerTester<QSet<T>> {
    static void iteration(KTextTemplate::Context &c)
    {
        KTextTemplate::Engine engine;

        engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

        KTextTemplate::Template t1 =
            engine.newTemplate(QStringLiteral("{% for person in people %}{{ person.name }},{% endfor %}"), QStringLiteral("people_template"));
        auto result = t1->render(&c);
        QStringList output{QStringLiteral("Claire,"), QStringLiteral("Grant,"), QStringLiteral("Alan,")};
        for (const QString &s : output) {
            QVERIFY(result.contains(s));
        }

        QCOMPARE(result.length(), output.join(QString()).length());
    }

    static void indexing(KTextTemplate::Context)
    {
    }
};

template<typename T>
struct SequentialContainerTester<std::list<T>> {
    static void iteration(KTextTemplate::Context &c)
    {
        testSequentialIteration<std::list<T>>(c);
    }

    static void indexing(KTextTemplate::Context)
    {
    }
};

template<typename Container>
void doTestSequentialContainer_Variant()
{
    KTextTemplate::Context c;

    insertPeopleVariants<Container>(c);

    SequentialContainerTester<Container>::iteration(c);
    SequentialContainerTester<Container>::indexing(c);
}

template<typename Container>
void testAssociativeValues(KTextTemplate::Context &c, bool unordered = {})
{
    KTextTemplate::Engine engine;

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    {
        KTextTemplate::Template t1 = engine.newTemplate(QStringLiteral("{% for person in people.values %}({{ person.name }}:{{ "
                                                                       "person.age }}),{% endfor %}"),
                                                        QStringLiteral("people_template"));

        auto result = t1->render(&c);
        if (!unordered)
            QCOMPARE(result, QStringLiteral("(Claire:23),(Grant:32),(Alan:50),"));
        else {
            QCOMPARE(result.size(), 33);
            QVERIFY(result.contains(QStringLiteral("(Claire:23),")));
            QVERIFY(result.contains(QStringLiteral("(Grant:32),")));
            QVERIFY(result.contains(QStringLiteral("(Alan:50),")));
        }
    }
}

template<typename Container>
void testAssociativeItems(KTextTemplate::Context &c, bool unordered)
{
    KTextTemplate::Engine engine;

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    {
        KTextTemplate::Template t1 = engine.newTemplate(QStringLiteral("{% for item in people.items %}({{ item.1.name }}:{{ "
                                                                       "item.1.age }}),{% endfor %}"),
                                                        QStringLiteral("people_template"));
        auto result = t1->render(&c);
        if (!unordered)
            QCOMPARE(result, QStringLiteral("(Claire:23),(Grant:32),(Alan:50),"));
        else {
            QCOMPARE(result.size(), 33);
            QVERIFY(result.contains(QStringLiteral("(Claire:23),")));
            QVERIFY(result.contains(QStringLiteral("(Grant:32),")));
            QVERIFY(result.contains(QStringLiteral("(Alan:50),")));
        }
    }
}

template<typename Container>
void doTestAssociativeContainer_Variant(bool unordered = {})
{
    KTextTemplate::Engine engine;

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    KTextTemplate::Context c;

    insertPeopleVariants<Container>(c);
    testAssociativeValues<Container>(c, unordered);
    testAssociativeItems<Container>(c, unordered);
}

void TestGenericTypes::testSequentialContainer_Variant()
{
    doTestSequentialContainer_Variant<QVariantList>();
    doTestSequentialContainer_Variant<QList<QVariant>>();
    doTestSequentialContainer_Variant<QStack<QVariant>>();
    doTestSequentialContainer_Variant<QQueue<QVariant>>();
}

void TestGenericTypes::testAssociativeContainer_Variant()
{
    doTestAssociativeContainer_Variant<QVariantMap>();
    doTestAssociativeContainer_Variant<QVariantHash>(true);
}

template<typename SequentialContainer>
void insertPeople(KTextTemplate::Context &c)
{
    auto people = getPeople();
    auto it = people.constBegin();
    const auto end = people.constEnd();
    SequentialContainer container;
    for (; it != end; ++it)
        container.insert(container.end(), it.value());
    c.insert(QStringLiteral("people"), QVariant::fromValue(container));
}

template<>
void insertPeople<QSet<Person>>(KTextTemplate::Context &c)
{
    auto people = getPeople();
    auto it = people.constBegin();
    const auto end = people.constEnd();
    QSet<Person> container;
    for (; it != end; ++it)
        container.insert(it.value());
    c.insert(QStringLiteral("people"), QVariant::fromValue(container));
}

template<>
void insertPeople<ThreeArray<Person>>(KTextTemplate::Context &c)
{
    auto people = getPeople();
    auto it = people.constBegin();
    ThreeArray<Person> container;
    for (auto i = 0; i < 3; ++i, ++it) {
        Q_ASSERT(it != people.constEnd());
        container[i] = it.value();
    }
    c.insert(QStringLiteral("people"), QVariant::fromValue(container));
}

template<typename AssociativeContainer>
void insertAssociatedPeople(KTextTemplate::Context &c)
{
    auto people = getPeople();
    auto it = people.constBegin();
    const auto end = people.constEnd();
    AssociativeContainer container;
    for (; it != end; ++it)
        container[QString::number(it.key())] = it.value();
    c.insert(QStringLiteral("people"), QVariant::fromValue(container));
}

template<typename AssociativeContainer>
void insertAssociatedPeople_Number(KTextTemplate::Context &c)
{
    auto people = getPeople();
    auto it = people.constBegin();
    const auto end = people.constEnd();
    AssociativeContainer container;
    for (; it != end; ++it)
        container[it.key()] = it.value();
    c.insert(QStringLiteral("people"), QVariant::fromValue(container));
}

template<typename Container>
void doTestSequentialContainer_Type()
{
    KTextTemplate::Context c;

    insertPeople<Container>(c);

    SequentialContainerTester<Container>::iteration(c);
    SequentialContainerTester<Container>::indexing(c);
}

template<typename Container>
void doTestAssociativeContainer_Type(bool unordered = {})
{
    KTextTemplate::Engine engine;

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    KTextTemplate::Context c;

    insertAssociatedPeople<Container>(c);
    testAssociativeValues<Container>(c, unordered);
    testAssociativeItems<Container>(c, unordered);
}

template<typename Container>
void doTestAssociativeContainer_Type_Number(bool unordered = {})
{
    KTextTemplate::Engine engine;

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    KTextTemplate::Context c;

    insertAssociatedPeople_Number<Container>(c);
    testAssociativeValues<Container>(c, unordered);
    testAssociativeItems<Container>(c, unordered);

    {
        KTextTemplate::Template t1 = engine.newTemplate(QStringLiteral("{{ people.23.name }}"), QStringLiteral("claire_template"));
        auto result = t1->render(&c);
        QCOMPARE(result, QStringLiteral("Claire"));
    }
}

void TestGenericTypes::testSequentialContainer_Type()
{
    doTestSequentialContainer_Type<QList<Person>>();
    doTestSequentialContainer_Type<QList<Person>>();
    doTestSequentialContainer_Type<QStack<Person>>();
    doTestSequentialContainer_Type<QQueue<Person>>();
    doTestSequentialContainer_Type<QSet<Person>>();
    doTestSequentialContainer_Type<std::deque<Person>>();
    doTestSequentialContainer_Type<std::vector<Person>>();
    doTestSequentialContainer_Type<std::list<Person>>();
    doTestSequentialContainer_Type<ThreeArray<Person>>();
}

void TestGenericTypes::testAssociativeContainer_Type()
{
    doTestAssociativeContainer_Type<QMap<QString, Person>>();
    doTestAssociativeContainer_Type_Number<QMap<qint16, Person>>();
    doTestAssociativeContainer_Type_Number<QMap<qint32, Person>>();
    doTestAssociativeContainer_Type_Number<QMap<qint64, Person>>();
    doTestAssociativeContainer_Type_Number<QMap<quint16, Person>>();
    doTestAssociativeContainer_Type_Number<QMap<quint32, Person>>();
    doTestAssociativeContainer_Type_Number<QMap<quint64, Person>>();
    doTestAssociativeContainer_Type<QHash<QString, Person>>(true);
    doTestAssociativeContainer_Type_Number<QHash<qint16, Person>>(true);
    doTestAssociativeContainer_Type_Number<QHash<qint32, Person>>(true);
    doTestAssociativeContainer_Type_Number<QHash<qint64, Person>>(true);
    doTestAssociativeContainer_Type_Number<QHash<quint16, Person>>(true);
    doTestAssociativeContainer_Type_Number<QHash<quint32, Person>>(true);
    doTestAssociativeContainer_Type_Number<QHash<quint64, Person>>(true);

    doTestAssociativeContainer_Type<std::map<QString, Person>>();
    doTestAssociativeContainer_Type_Number<std::map<qint16, Person>>();
    doTestAssociativeContainer_Type_Number<std::map<qint32, Person>>();
    doTestAssociativeContainer_Type_Number<std::map<qint64, Person>>();
    doTestAssociativeContainer_Type_Number<std::map<quint16, Person>>();
    doTestAssociativeContainer_Type_Number<std::map<quint32, Person>>();
    doTestAssociativeContainer_Type_Number<std::map<quint64, Person>>();

    doTestAssociativeContainer_Type<QtUnorderedMap<QString, Person>>(true);
    doTestAssociativeContainer_Type_Number<QtUnorderedMap<qint16, Person>>(true);
    doTestAssociativeContainer_Type_Number<QtUnorderedMap<qint32, Person>>(true);
    doTestAssociativeContainer_Type_Number<QtUnorderedMap<qint64, Person>>(true);
    doTestAssociativeContainer_Type_Number<QtUnorderedMap<quint16, Person>>(true);
    doTestAssociativeContainer_Type_Number<QtUnorderedMap<quint32, Person>>(true);
    doTestAssociativeContainer_Type_Number<QtUnorderedMap<quint64, Person>>(true);
}

void TestGenericTypes::testSharedPointer()
{
    KTextTemplate::Engine engine;

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    auto t1 = engine.newTemplate(QStringLiteral("{{ p.name }} {{ p.age }}"), QStringLiteral("template1"));

    // Check it
    QVariantHash h;
    QSharedPointer<PersonObject> p(new PersonObject(QStringLiteral("Grant Lee"), 2));
    h.insert(QStringLiteral("p"), QVariant::fromValue(p));
    KTextTemplate::Context c(h);
    QCOMPARE(t1->render(&c), QStringLiteral("Grant Lee 2"));
}

void TestGenericTypes::testThirdPartySharedPointer()
{
    KTextTemplate::Engine engine;

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    auto t1 = engine.newTemplate(QStringLiteral("{{ p.name }} {{ p.age }}"), QStringLiteral("template1"));

    // Check it
    QVariantHash h;
    std::shared_ptr<PersonObject> p(new PersonObject(QStringLiteral("Grant Lee"), 2));
    h.insert(QStringLiteral("p"), QVariant::fromValue(p));
    KTextTemplate::Context c(h);
    QCOMPARE(t1->render(&c), QStringLiteral("Grant Lee 2"));
}

using ListVectorInt = QList<QList<qint16>>;
using MapListVectorInt = QMap<int, QList<QList<qint16>>>;
using StackMapListVectorInt = QStack<QMap<int, QList<QList<qint16>>>>;

static QList<qint16> getNumbers()
{
    static auto n = 0;
    QList<qint16> nums;
    nums.push_back(++n);
    nums.push_back(++n);
    return nums;
}

static ListVectorInt getNumberLists()
{
    ListVectorInt list;
    for (auto i = 0; i < 2; ++i) {
        list.append(getNumbers());
    }
    return list;
}

static MapListVectorInt getNumberListMap()
{
    MapListVectorInt map;
    for (auto i = 0; i < 2; ++i) {
        map.insert(i, getNumberLists());
    }
    return map;
}

static StackMapListVectorInt getMapStack()
{
    StackMapListVectorInt stack;
    for (auto i = 0; i < 2; ++i) {
        stack.push(getNumberListMap());
    }
    return stack;
}

void TestGenericTypes::testNestedContainers()
{
    KTextTemplate::Engine engine;

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    KTextTemplate::Context c;
    c.insert(QStringLiteral("stack"), QVariant::fromValue(getMapStack()));

#if defined(Q_CC_MSVC)
// MSVC doesn't like static string concatenations like L"foo" "bar", as
// results from QStringLiteral, so use QLatin1String here instead.
#define STRING_LITERAL QLatin1String
#else
#define STRING_LITERAL QStringLiteral
#endif
    auto t1 = engine.newTemplate(STRING_LITERAL("{% for map in stack %}"
                                                "(M {% for key, list in map.items %}"
                                                "({{ key }} : (L {% for vector in list %}"
                                                "(V {% for number in vector %}"
                                                "{{ number }},"
                                                "{% endfor %}),"
                                                "{% endfor %}),"
                                                "{% endfor %}),"
                                                "{% endfor %}"),
                                 QStringLiteral("template1"));

#undef STRING_LITERAL

    auto result = t1->render(&c);

    auto expectedResult = QStringLiteral(
        "(M (0 : (L (V 1,2,),(V 3,4,),),(1 : (L (V 5,6,),(V 7,8,),),),(M (0 : (L "
        "(V 9,10,),(V 11,12,),),(1 : (L (V 13,14,),(V 15,16,),),),");

    QCOMPARE(result, expectedResult);
}

class CustomObject : public QObject
{
    Q_OBJECT
public:
    explicit CustomObject(QObject *parent = {})
        : QObject(parent)
    {
    }
};

class OtherObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(CustomObject *custom READ custom)
public:
    explicit OtherObject(QObject *parent = {})
        : QObject(parent)
        , m_custom(new CustomObject(this))
    {
        m_custom->setProperty("nestedProp", QStringLiteral("nestedValue"));
    }

    CustomObject *custom()
    {
        return m_custom;
    }

private:
    CustomObject *m_custom;
};

void TestGenericTypes::testCustomQObjectDerived()
{
    KTextTemplate::Engine engine;

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    auto customObject = new CustomObject(this);
    customObject->setProperty("someProp", QStringLiteral("propValue"));

    KTextTemplate::Context c;
    c.insert(QStringLiteral("custom"), QVariant::fromValue(customObject));

    {
        auto t1 = engine.newTemplate(QStringLiteral("{{ custom.someProp }}"), QStringLiteral("template1"));

        auto result = t1->render(&c);
        auto expectedResult = QStringLiteral("propValue");

        QCOMPARE(result, expectedResult);
    }

    auto other = new OtherObject(this);

    c.insert(QStringLiteral("other"), other);

    {
        auto t1 = engine.newTemplate(QStringLiteral("{{ other.custom.nestedProp }}"), QStringLiteral("template1"));

        auto result = t1->render(&c);
        auto expectedResult = QStringLiteral("nestedValue");

        QCOMPARE(result, expectedResult);
    }
}

struct UnregisteredType {
};

Q_DECLARE_METATYPE(UnregisteredType)

struct RegisteredNotListType {
};

Q_DECLARE_METATYPE(RegisteredNotListType)

KTEXTTEMPLATE_BEGIN_LOOKUP(RegisteredNotListType)
Q_UNUSED(object)
if (property == QStringLiteral("property"))
    return 42;
KTEXTTEMPLATE_END_LOOKUP

static QVariantList dummy(const UnregisteredType &)
{
    return QVariantList{42};
}

QVariant dummyLookup(const QVariant &, const QString &)
{
    return 42;
}

void TestGenericTypes::testUnregistered()
{
    {
        UnregisteredType unregType;
        auto v = QVariant::fromValue(unregType);

        auto result = KTextTemplate::MetaType::lookup(v, QStringLiteral("property"));
        QVERIFY(!result.isValid());

        QVERIFY(!v.canConvert<QVariantList>());
    }

    KTextTemplate::registerMetaType<RegisteredNotListType>();

    {
        RegisteredNotListType nonListType;
        auto v = QVariant::fromValue(nonListType);
        auto result = KTextTemplate::MetaType::lookup(v, QStringLiteral("property"));
        QVERIFY(result.isValid());
        QVERIFY(!v.canConvert<QVariantList>());
    }

    {
        QMetaType::registerConverter<UnregisteredType, QVariantList>(&dummy);
        UnregisteredType unregType;
        auto v = QVariant::fromValue(unregType);
        auto result = KTextTemplate::MetaType::lookup(v, QStringLiteral("property"));
        QVERIFY(!result.isValid());
    }

    // Only do this in release mode?
    //   KTextTemplate::MetaType::registerLookUpOperator(0, dummyLookup);
    //   KTextTemplate::MetaType::registerToVariantListOperator(0, dummy);
}

Q_DECLARE_METATYPE(Person *)

KTEXTTEMPLATE_BEGIN_LOOKUP_PTR(Person)
if (property == QStringLiteral("name"))
    return QString::fromStdString(object->name);
if (property == QStringLiteral("age"))
    return object->age;
KTEXTTEMPLATE_END_LOOKUP

void TestGenericTypes::testPointerNonQObject()
{
    auto p = new Person("Adele", 21);
    auto v = QVariant::fromValue(p);

    KTextTemplate::registerMetaType<Person *>();

    auto result = KTextTemplate::MetaType::lookup(v, QStringLiteral("name"));

    QCOMPARE(result.value<QString>(), QStringLiteral("Adele"));

    delete p;
}

class CustomGadget
{
    Q_GADGET
    Q_PROPERTY(int fortyTwo READ fortyTwo)
public:
    int fortyTwo()
    {
        return 42;
    }
};

void TestGenericTypes::testQGadget()
{
    CustomGadget g;
    auto v = QVariant::fromValue(g);

    auto result = KTextTemplate::MetaType::lookup(v, QStringLiteral("fortyTwo"));

    QCOMPARE(result.value<int>(), 42);
}

void TestGenericTypes::testGadgetMetaType()
{
    KTextTemplate::Engine engine;
    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    auto t1 = engine.newTemplate(QStringLiteral("Person: \nName: {{p.name}}\nAge: {{p.age}}"), QStringLiteral("template1"));

    PersonGadget p;
    p.m_name = QStringLiteral("Some Name");
    KTextTemplate::Context c;
    c.insert(QStringLiteral("p"), QVariant::fromValue(p));
    QCOMPARE(t1->render(&c), QStringLiteral("Person: \nName: Some Name\nAge: 42"));
}

class ObjectWithProperties : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<int> numberList READ numberList)
    Q_PROPERTY(QList<CustomGadget> gadgetList READ gadgetList)
    Q_PROPERTY(QList<PersonObject *> personList READ personList)
    Q_PROPERTY(QList<QSharedPointer<PersonObject>> personPtrList READ personPtrList)

public:
    ObjectWithProperties(QObject *parent = {})
        : QObject(parent)
    {
        m_numberList.push_back(42);
        m_numberList.push_back(7);
        m_gadgetList.push_back(CustomGadget{});
        m_gadgetList.push_back(CustomGadget{});
        m_personList.push_back(new PersonObject{QStringLiteral("Joe"), 20, this});
        m_personList.push_back(new PersonObject{QStringLiteral("Mike"), 22, this});
        m_personPtrList.push_back(QSharedPointer<PersonObject>::create(QStringLiteral("Niall"), 23));
        m_personPtrList.push_back(QSharedPointer<PersonObject>::create(QStringLiteral("Dave"), 24));
    }

    QList<int> numberList()
    {
        return m_numberList;
    }
    QList<CustomGadget> gadgetList()
    {
        return m_gadgetList;
    }
    QList<PersonObject *> personList()
    {
        return m_personList;
    }
    QList<QSharedPointer<PersonObject>> personPtrList()
    {
        return m_personPtrList;
    }

private:
    QList<int> m_numberList;
    QList<CustomGadget> m_gadgetList;
    QList<PersonObject *> m_personList;
    QList<QSharedPointer<PersonObject>> m_personPtrList;
};

void TestGenericTypes::propertyMacroTypes()
{
    KTextTemplate::Engine engine;

    qRegisterMetaType<QList<CustomGadget>>();

    engine.setPluginPaths({QStringLiteral(KTEXTTEMPLATE_PLUGIN_PATH)});

    auto objectWithProperties = new ObjectWithProperties(this);

    KTextTemplate::Context c;
    c.insert(QStringLiteral("obj"), objectWithProperties);

    {
        auto t1 = engine.newTemplate(QStringLiteral("{{ obj.numberList.0 }}--{{ obj.numberList.1 }}"), QStringLiteral("template1"));

        auto result = t1->render(&c);
        auto expectedResult = QStringLiteral("42--7");

        QCOMPARE(result, expectedResult);
    }

    {
        auto t1 = engine.newTemplate(QStringLiteral("{{ obj.gadgetList.0.fortyTwo }}--{{ obj.gadgetList.1.fortyTwo }}"), QStringLiteral("template1"));

        auto result = t1->render(&c);
        auto expectedResult = QStringLiteral("42--42");

        QCOMPARE(result, expectedResult);
    }

    {
        auto t1 = engine.newTemplate(QStringLiteral("{{ obj.personList.0.name }}({{ obj.personList.0.age }})"
                                                    "--{{ obj.personList.1.name }}({{ obj.personList.1.age }})"),
                                     QStringLiteral("template1"));

        auto result = t1->render(&c);
        auto expectedResult = QStringLiteral("Joe(20)--Mike(22)");

        QCOMPARE(result, expectedResult);
    }

    {
        auto t1 = engine.newTemplate(QStringLiteral("{{ obj.personPtrList.0.name }}({{ obj.personPtrList.0.age }})"
                                                    "--{{ obj.personPtrList.1.name }}({{ obj.personPtrList.1.age }})"),
                                     QStringLiteral("template1"));

        auto result = t1->render(&c);
        auto expectedResult = QStringLiteral("Niall(23)--Dave(24)");

        QCOMPARE(result, expectedResult);
    }
}

QTEST_MAIN(TestGenericTypes)
#include "testgenerictypes.moc"

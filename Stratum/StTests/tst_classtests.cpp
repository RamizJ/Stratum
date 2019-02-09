#include <QtTest>
#include <memory>

#include <Class.h>

using namespace StData;

class ClassTests : public QObject
{
    Q_OBJECT

public:
    ClassTests()
    {}
    ~ClassTests()
    {}

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testCaseOne();
    void testCaseTwo();

private:
    std::shared_ptr<Class> m_class;
};



void ClassTests::initTestCase()
{}

void ClassTests::cleanupTestCase()
{}



void ClassTests::testCaseOne()
{}

void ClassTests::testCaseTwo()
{}



QTEST_APPLESS_MAIN(ClassTests)

#include "tst_classtests.moc"
//#include "tst_vmtests.moc"

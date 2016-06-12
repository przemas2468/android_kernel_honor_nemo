#include "gtest/gtest.h"
#include "llt_mockcpp.h"

#include "uttest_nas_RnicDebug.h"

//建议这样引用，避免下面用关键字时需要加前缀 testing::
using namespace testing;

//说明: 经过改造后的gtest+，测试套和测试用例名字不允许为空
//      请避免用和gtest相同的关键字，避免不必要的麻烦
//      gtest提供 EXPECT 和 ASSERT 两套,具体含义可以看胶片，两套宏都是一一对应的，下面以EXPECT为例

//1、比对布尔类型 EXPECT_TRUE　EXPECT_FALSE/ASSERT_TRUE
//2、比较整形 EXPECT_EQ EXPECT_NE
//3、比较字符串 EXPECT_STREQ
//4、设置失败输出信息 ADD_FAILUREd
//5、设置成功和失败 SUCCEED FAIL
//6、自定义格式 EXPECT_PRED_FORMAT3
//7、期望带入参数返回TRUE EXPECT_PRED1
//8、浮点型比较 EXPECT_FLOAT_EQ EXPECT_PRED_FORMAT2 ASSERT_PRED_FORMAT2
//9、类型检查 StaticAssertTypeEq
//10、设置3种测试级别事件
//11、获取测试用例名字 test_case_name()
//12、比较异常 EXPECT_THROW
//13、结构体类型比较ASSERT_SAME_DATA ASSERT_SAME_MEMORY

#if 0


class Test_RNIC_Help: public ::testing::Test
{
public:
    void SetUp()
	{
		UT_STUB_INIT();
    }
    void TearDown()
    {
    }
};


TEST_F(Test_RNIC_Help, Test_RNIC_Help_001)
{
    //参数定义

    //参数初始化

    //相关全局变量初始化

    //MOCKER操作

    //调用被测函数
    RNIC_Help();

    //执行检查

    //检查调用时, 需要在用例结束位置加上这句
    GlobalMockObject::verify();
}


class Test_RNIC_ShowResetStats: public ::testing::Test
{
public:
    void SetUp()
	{
		UT_STUB_INIT();
    }
    void TearDown()
    {
    }
};


TEST_F(Test_RNIC_ShowResetStats, Test_RNIC_ShowResetStats_001)
{
    //参数定义

    //参数初始化

    //相关全局变量初始化

    //MOCKER操作

    //调用被测函数
    RNIC_ShowResetStats();

    //执行检查

    //检查调用时, 需要在用例结束位置加上这句
    GlobalMockObject::verify();
}

#endif

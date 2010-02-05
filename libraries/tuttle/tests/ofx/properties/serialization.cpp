#include <tuttle/common/utils/global.hpp>
#include <tuttle/host/core/Core.hpp>

#include <boost/archive/xml_oarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/serialization.hpp>
#include <fstream>

#include <iostream>

#include <boost/test/unit_test.hpp>
using namespace boost::unit_test;


BOOST_AUTO_TEST_SUITE( properties_tests_suite02 )

BOOST_AUTO_TEST_CASE( properties_serialization )
{
	using namespace std;
	using namespace tuttle::host;

#define testString_1_ro "testString_1_ro"
#define testString_2_rw "testString_2_rw"
#define testPointer_1_rw "testPointer_1_rw"
#define testDouble_2_ro "testDouble_2_ro"
#define testDouble_1_rw "testDouble_1_rw"
#define testInt_1_rw "testInt_1_rw"
#define testInt_2_rw "testInt_2_rw"
#define testInt_4_ro "testInt_4_ro"

	static const ofx::property::OfxhPropSpec testStuff[] = {
		/* name                                 type                   dim.   r/o    default value */
		{ testString_1_ro, ofx::property::eString, 1, true, "default_value" },
		{ testString_2_rw, ofx::property::eString, 2, false, "default_value" },
		{ testPointer_1_rw, ofx::property::ePointer, 1, false, NULL },
		{ testDouble_2_ro, ofx::property::eDouble, 2, true, "0" },
		{ testDouble_1_rw, ofx::property::eDouble, 1, false, "0" },
		{ testInt_1_rw, ofx::property::eInt, 1, false, "0" },
		{ testInt_2_rw, ofx::property::eInt, 2, false, "8" },
		{ testInt_4_ro, ofx::property::eInt, 4, true, "0" },
		{ 0 }
	};

	// init values
	ofx::property::OfxhSet testSet( testStuff );
	testSet.setStringProperty( testString_1_ro, "testcontext" );
	testSet.setIntProperty( testInt_1_rw, true );
	int tab4Int[4] = { 11, 22, 33, 44 };
	testSet.setIntProperty( testInt_4_ro, tab4Int[0], 0 );
	testSet.setIntProperty( testInt_4_ro, tab4Int[1], 1 );
	testSet.setIntProperty( testInt_4_ro, tab4Int[2], 2 );
	testSet.setIntProperty( testInt_4_ro, tab4Int[3], 3 );
	double tab2Double[2] = { 11, 22 };
	testSet.setDoublePropertyN( testDouble_2_ro, tab2Double, 2 );


//	typedef boost::archive::binary_oarchive OArchive;
//	typedef boost::archive::binary_iarchive IArchive;
//	typedef boost::archive::text_oarchive OArchive;
//	typedef boost::archive::text_iarchive IArchive;
	typedef boost::archive::xml_oarchive OArchive;
	typedef boost::archive::xml_iarchive IArchive;

//    std::string testfile( boost::archive::tmpnam(NULL) );
    std::string testfile( "test_properties_serialization.xml" );
    BOOST_REQUIRE( testfile.size() );

	std::ofstream ofsb( testfile.c_str() );
	OArchive oArchive( ofsb );
	oArchive << BOOST_SERIALIZATION_NVP(testSet);
	ofsb.close();

//    std::remove(testfile);
}

BOOST_AUTO_TEST_SUITE_END()

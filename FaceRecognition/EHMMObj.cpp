#include "EHMMObj.h"

using std::string;

//Standard extension to the user file
static const string EXTENSION = ".ehmm";

//*****************************************************************************

EHMMObj::EHMMObj( )
{
}

//*****************************************************************************

EHMMObj::~EHMMObj( )
{
}

//*****************************************************************************

void EHMMObj::Create( const string &userName, int *noStates, 
					  int *noMix, int vecSize )
{
	assert( _name == "" );

	Obj::Create( userName );

	//Create the ehmm for the object
	_ehmm.Create( noStates, noMix, vecSize );
}

//*****************************************************************************

void EHMMObj::Load( const string &userName, const string &path )
{
        std::cout<<"\nEHMM Passed Path:"<<path; fflush(stdout);
        std::cout<<"\nEHMM Passed UserName:"<<userName; fflush(stdout);
        string EHMM_FILE =  path + "/" + userName ;//+ ".ehmm";//EXTENSION;
        std::cout<<"\nEHMM Name:"<< EHMM_FILE; fflush(stdout);
        //EHMM_FILE[EHMM_FILE.size()-1] = '\0';
        EHMM_FILE = EHMM_FILE + EXTENSION;
        assert( _name == "" );

	Obj::Create( userName );

	//Load the ehmm for each of the users
	_ehmm.Load( EHMM_FILE );
}

//*****************************************************************************

void EHMMObj::Save( const string &path )
{
	const string EHMM_FILE = path + _name + EXTENSION;

	assert( _name != "" );

	//Save the ehmm for the object
	_ehmm.Save( EHMM_FILE );
}

//*****************************************************************************

EHMM& EHMMObj::GetEHMM( )
{
	return _ehmm;
}

//*****************************************************************************

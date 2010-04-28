#include "../include/mbuffer.h"
using namespace std;
inline UINT32 MBuffer::CalcNextSize( UINT32 _nWishedSize )
{
	return ( _nWishedSize + 1023 ) & 0xFFFFFC00;	// block size is 1KB
}

inline UINT32 MBuffer::_BytesLeft( void ) const
{
	return mnStrEnd - mnStrPos;
}

void MBuffer::Init( UINT32 _nInitSize )
{
	mnBufferSize = CalcNextSize( _nInitSize? _nInitSize : 1 );
	mpBuffer = new char[ mnBufferSize ];
	mnStrPos = mnStrEnd = 0;
	mnMaxSize = CalcNextSize( 8192 );	// just max. 8KB of size might be enough for the near future
	mbInStreamWriting = true;
	mbAlwaysAppend = false;
	mbBigEndian = false;
}

bool MBuffer::Grow( UINT32 _nNewSize )
{
     

	bool	bSuccGrowed = true;

	if( _nNewSize > mnBufferSize )
	{
		UINT32	nNewSize = CalcNextSize( _nNewSize );

		bSuccGrowed = nNewSize <= mnMaxSize;
		if( !bSuccGrowed )
			nNewSize = mnMaxSize;

		char*	pNewData = new char[ nNewSize ];
		memcpy( pNewData, mpBuffer, mnStrEnd );
		delete[] mpBuffer;
		mpBuffer = pNewData;
		mnBufferSize = nNewSize; 

	}

	return bSuccGrowed;
}

MBuffer::MBuffer( void )
{
	Init( 0 );
}

MBuffer::MBuffer( const UINT32 _nInitSize )
{
	Init( _nInitSize );
}

MBuffer::~MBuffer()
{
	delete[] mpBuffer;
}

UINT32 MBuffer::Write( const void* _pData, UINT32 _nNumOfBytes )
{
	if( mbAlwaysAppend )
		Seek( SEEKTOENDPOS );

	if( _nNumOfBytes > ( mnMaxSize - mnStrPos ) )
		return 0;

	// now there is no more need to check if further calculations will overflow because of previous check
	UINT32	nNewStrPos = mnStrPos + _nNumOfBytes;
	if( nNewStrPos >= mnBufferSize )
		Grow( nNewStrPos );

	// now all is ready for the copy action
	memcpy( mpBuffer + mnStrPos, _pData, _nNumOfBytes );
	mnStrPos = nNewStrPos;
/*	if( mbInStreamWriting )
	{
		if( mnStrPos > mnStrEnd )
			mnStrEnd = mnStrPos;
	}
	else
		mnStrEnd = mnStrPos;  -> */
	if( !mbInStreamWriting || mnStrPos > mnStrEnd )
		mnStrEnd = mnStrPos;

	return _nNumOfBytes;
}

UINT32 MBuffer::Write( UINT8 _nVal )
{
	// quick implementation with potential for improvements... ;-)
	return Write( ( const char* ) &_nVal, sizeof( _nVal ) );
}

UINT32 MBuffer::Write( UINT16 _nVal )
{
	// quick implementation with potential for improvements... ;-)
/*	if( mbBigEndian )
		_nVal = Util::GetBigEndian( _nVal );
	else 
		_nVal = Util::GetLittleEndian( _nVal );*/
	return Write( ( const char* ) &_nVal, sizeof( _nVal ) );
}

UINT32 MBuffer::Write( UINT32 _nVal )
{
	// quick implementation with potential for improvements... ;-)
/*	if( mbBigEndian )
		_nVal = Util::GetBigEndian( _nVal );
	else
		_nVal = Util::GetLittleEndian( _nVal );*/
	return Write( ( const char* ) &_nVal, sizeof( _nVal ) );
}

UINT32 MBuffer::Write( UINT64 _nVal )
{
	// quick implementation with potential for improvements... ;-)
/*	if( mbBigEndian )
		_nVal = Util::GetBigEndian( _nVal );
	else
		_nVal = Util::GetLittleEndian( _nVal );*/
	return Write( ( const char* ) &_nVal, sizeof( _nVal ) );
}

UINT32 MBuffer::Write( double _fVal )
{
	// quick implementation with potential for improvements... ;-)
/*	if( mbBigEndian )
		_fVal = Util::GetBigEndian( _fVal );
	else
		_fVal = Util::GetLittleEndian( _fVal );*/
	return Write( ( const char* ) &_fVal, sizeof( _fVal ) );
}
/*
UINT32 MBuffer::Write( const String& _rStr, StrLenWidth _e, UINT32 _nMaxLen )
{
	UINT32		nRet = 0;
	switch( _e )
	{
		case SLW_1:
			{
				UINT32	nRealLen = _rStr.Len();
				if( _nMaxLen > 0xFF )
					_nMaxLen = 0xFF;
				if( nRealLen > _nMaxLen )
					nRealLen = _nMaxLen;

				Write( UINT8( nRealLen ) );
				Write( ( const char* ) _rStr, nRealLen );

				nRet = nRealLen + 1;
			}
			break;
		case SLW_2:
			{
				UINT32	nRealLen = _rStr.Len();
				if( _nMaxLen > 0xFFFF )
					_nMaxLen = 0xFFFF;
				if( nRealLen > _nMaxLen )
					nRealLen = _nMaxLen;

				Write( UINT16( nRealLen ) );
				Write( ( const char* ) _rStr, nRealLen );

				nRet = nRealLen + 1;
			}
			break;
	}

	return nRet;
}
*/
UINT32 MBuffer::Read( void* _pData, UINT32 _nNumOfBytes )
{
	UINT32	nRealBytesRead = ( _BytesLeft() >= _nNumOfBytes )? _nNumOfBytes : _BytesLeft();

	memcpy( _pData, mpBuffer + mnStrPos, nRealBytesRead );
	mnStrPos += nRealBytesRead;

	return nRealBytesRead;
}

#define READ_IMPL(typeofval)								\
	typeofval	nVal;										\
	UINT32	nNumOfBytes = Read( &nVal, sizeof( nVal ) );	\
	if( nNumOfBytes < sizeof( nVal ) )						\
		nNumOfBytes = 0;									\
	else													\
		_rVal = nVal;										\
	return nNumOfBytes										


UINT32 MBuffer::Read( UINT8& _rVal )
{
	// quick implementation with potential for improvements... ;-)
	READ_IMPL( UINT8 );
}

UINT32 MBuffer::Read( UINT16& _rVal )
{
	// quick implementation with potential for improvements... ;-)
//	READ_IMPL( UINT16 );
	UINT16	nVal;
	UINT32	nNumOfBytes = Read( &nVal, sizeof( nVal ) );
	if( nNumOfBytes < sizeof( nVal ) )
		nNumOfBytes = 0;
	else
	{
	/*	if( mbBigEndian )
			_rVal = Util::GetBigEndian( nVal );
		else
			_rVal = Util::GetLittleEndian( nVal );*/
			_rVal =nVal;
	}

	return nNumOfBytes;
}

UINT32 MBuffer::Read( UINT32& _rVal )
{
	// quick implementation with potential for improvements... ;-)
//	READ_IMPL( UINT32 );
	UINT32	nVal;
	UINT32	nNumOfBytes = Read( &nVal, sizeof( nVal ) );
	if( nNumOfBytes < sizeof( nVal ) )
		nNumOfBytes = 0;
	else
	{
	/*	if( mbBigEndian )
			_rVal = Util::GetBigEndian( nVal );
		else
			_rVal = Util::GetLittleEndian( nVal );*/
			_rVal =nVal;
	}

	return nNumOfBytes;
}

UINT32 MBuffer::Read( UINT64& _rVal )
{
	// quick implementation with potential for improvements... ;-)
	READ_IMPL( UINT64 );
}

UINT32 MBuffer::Read( double& _rVal )
{
	// quick implementation with potential for improvements... ;-)
	READ_IMPL( double );
}
/*
UINT32 MBuffer::Read( String& _rStr, StrLenWidth _e )
{
	UINT32		nRet = 0;
	switch( _e )
	{
		case SLW_1:
			{
				UINT8	nLen;
				UINT8	c;
				nRet += Read( nLen );
				while( nLen )
				{
					nRet += Read( c );
					_rStr += char( c );
					--nLen;
				}
			}
			break;
	}

	return nRet;
}

MBuffer& MBuffer::operator <<( char _c )
{
	Write( &_c, 1 );
	return *this;
}

MBuffer& MBuffer::operator <<( UINT16 _nVal )
{
	String	s;
	s += UINT32( _nVal );
	Write( ( const char* ) s, s.Len() );
	return *this;
}

MBuffer& MBuffer::operator <<( UINT32 _nVal )
{
	String	s;
	s += _nVal;
	Write( ( const char* ) s, s.Len() );
	return *this;
}

MBuffer& MBuffer::operator <<( UINT64 _nVal )
{
	String	s;
	s += _nVal;
	Write( ( const char* ) s, s.Len() );
	return *this;
}

MBuffer& MBuffer::operator <<( const char* _pCStr )
{
	Write( _pCStr, String::Len( _pCStr ) );
	return *this;
}
*/
bool MBuffer::IsEOS( void ) const
{
	return mnStrPos >= mnStrEnd;
}

void MBuffer::Seek( UINT32 _nStrPos )
{
	mnStrPos = ( _nStrPos > mnStrEnd )? mnStrEnd : _nStrPos;
}

UINT32 MBuffer::Tell( void ) const
{
	return mnStrPos;
}

UINT32 MBuffer::GetLen( void ) const
{
	return mnStrEnd;
}

UINT32 MBuffer::GetBytesLeft( void ) const
{
	return _BytesLeft();
}

UINT32 MBuffer::GetRepLen( void ) const
{
	return mnBufferSize;
}

void MBuffer::Clear( void )
{
	mnStrPos = mnStrEnd = 0;
}

#include <iostream>
#include <cstring>

typedef unsigned char	UINT8;
typedef signed char		INT8;
typedef unsigned short	UINT16;
typedef signed short	INT16;
typedef unsigned int	UINT32;
typedef signed int		INT32;
typedef unsigned long long UINT64;
typedef signed long long INT64;
typedef unsigned char	BYTE;




// die eigentliche Klasse

#define	SEEKTOENDPOS		0xFFFFFFFF

//enum StrLenWidth			{ SLW_1, SLW_2 };

class MBuffer
{
private:
	void					Init( UINT32 _nInitSize );
	static inline UINT32	CalcNextSize( UINT32 _nWishedSize );
	bool					Grow( UINT32 _nNewSize );
	inline UINT32			_BytesLeft( void ) const;
protected:
	char*					    mpBuffer;			// pointer to data array
	UINT32					mnBufferSize;		// size of buffer
	UINT32					mnStrPos;			// actual stream pos
	UINT32					mnStrEnd;			// first byte after the stream
	UINT32					mnMaxSize;			// max. size of stream
	bool					        mbInStreamWriting;	// if true, a Write() in the middle of the stream will not change the
												 //	stream length (-> will not change the content behind the write pos)
												//	default = true
	bool					        mbAlwaysAppend;		// if true, Write() will always append no matter where the stream pos
												//	is actually!
												//	default = false
	bool					        mbBigEndian;		// Byte order
public:
							MBuffer( void );
							MBuffer( const UINT32 _nInitSize );
	virtual			~MBuffer();

							// return value is always the actual stream pos, 0 if an error occured
							// be carefull: both Write- and Read-methods change the actual stream pos!
	UINT32					Write( const void* _pData, UINT32 _nNumOfBytes );
	UINT32					Write( UINT8 _nVal );
	UINT32					Write( UINT16 _nVal );
	UINT32					Write( UINT32 _nVal );
	UINT32					Write( UINT64 _nVal );
	UINT32					Write( double _fVal );

//	UINT32					Write( const String& _rStr, StrLenWidth _eStrLenWidth, UINT32 _nMaxLen = 0xFFFFFFFF );

	UINT32					Read( void* _pData, UINT32 _nNumOfBytes );
								// return value is the number of Bytes which could be read

							// returns 0 if not all Bytes could be read, otherwise the full number of read Bytes
							//	if a value could not be read completely, _rVal is _not_ changed!
	UINT32					Read( UINT8& _rVal );
	UINT32					Read( UINT16& _rVal );
	UINT32					Read( UINT32& _rVal );
	UINT32					Read( UINT64& _rVal );
	UINT32					Read( double& _rVal );

//	UINT32					Read( String& _rStr, StrLenWidth _eStrLenWidth );

	// ASCII access
	/*
	MBuffer&				operator <<( char _c );
	MBuffer&				operator <<( UINT16 _nVal );
	MBuffer&				operator <<( UINT32 _nVal );
	MBuffer&				operator <<( UINT64 _nVal );
	MBuffer&				operator <<( const char* _pCStr );
    */
	bool					IsEOS( void ) const;

	void					Seek( UINT32 _nStrPos );
								// if _nStrPos is larger than stream size, the actual stream pos will be at the end of the stream
	UINT32					Tell( void ) const;
	UINT32					GetLen( void ) const;
	UINT32					GetBytesLeft( void ) const;		// number of Bytes left for beeing read
	UINT32 					GetRepLen(void) const;

	inline const char*		GetBuffer( void ) const;

	inline void				SetInStreamWriting( bool _bEnable );
	inline void				SetAlwaysAppend( bool _bEnable );

	inline void				SetBigEndianMode( bool _bBigEndianEnable = true );

	void					Clear( void );
};



inline void MBuffer::SetInStreamWriting( bool _b )
{
	mbInStreamWriting = _b;
}


inline const char* MBuffer::GetBuffer( void ) const
{
	return mpBuffer;
}


inline void MBuffer::SetAlwaysAppend( bool _b )
{
	mbAlwaysAppend = _b;
}

inline void MBuffer::SetBigEndianMode( bool _b )
{
	mbBigEndian = _b;
}

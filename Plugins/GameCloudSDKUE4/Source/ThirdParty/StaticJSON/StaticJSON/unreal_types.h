// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "StaticJSON/staticjson.hpp"
#include "UnrealType.h"

namespace staticjson
{
	//======================================================================//
	//								FString									//
	//======================================================================//
	template <>
	class Handler<FString> : public BaseHandler
	{
	private:
		FString* m_value;

	public:
		explicit Handler( FString* v ) : m_value( v ) {}

		bool String( const char* str, SizeType length, bool ) override
		{
			*m_value = FString( str );
			this->parsed = true;
			return true;
		}

		std::string type_name() const override { return "FString"; }

		bool write( IHandler* out ) const override
		{
			std::string tempStr( TCHAR_TO_UTF8( **m_value ) );
			return out->String( tempStr.data(), SizeType( tempStr.size() ), true );
		}

		void generate_schema( Value& output, MemoryPoolAllocator& alloc ) const override
		{
			output.SetObject();
			output.AddMember( rapidjson::StringRef( "type" ), rapidjson::StringRef( "FString" ), alloc );
		}
	};

	//======================================================================//
	//								TArray									//
	//======================================================================//
	template <typename ArrayType>
	class UnrealArrayHandler : public BaseHandler
	{
	public:
		typedef typename ArrayType::ElementType ElementType;

	protected:
		ElementType element;					// the type that is passed into array, int, float, bool, etc.
		Handler<ElementType> internal;			// becomes an int, float, bool, etc.
		ArrayType* m_value;						// the array
		int depth = 0;

	protected:
		void set_element_error() { the_error.reset( new error::ArrayElementError( m_value->Num() ) ); }

		bool precheck( const char* type )
		{
			if ( depth <= 0 )
			{
				the_error.reset( new error::TypeMismatchError( type_name(), type ) );
				return false;
			}
			return true;
		}

		bool postcheck( bool success )
		{
			if ( !success )
			{
				set_element_error();
				return false;
			}
			if ( internal.is_parsed() )
			{
				m_value->Emplace( std::move( element ) );
				element = ElementType();
				internal.prepare_for_reuse();
			}
			return true;
		}

		void reset() override
		{
			element = ElementType();
			internal.prepare_for_reuse();
			depth = 0;
		}

	public:
		explicit UnrealArrayHandler( ArrayType* value ) : element(), internal( &element ), m_value( value ) {}

		bool Null() override { return precheck( "null" ) && postcheck( internal.Null() ); }

		bool Bool( bool b ) override { return precheck( "bool" ) && postcheck( internal.Bool( b ) ); }

		bool Int( int i ) override { return precheck( "int" ) && postcheck( internal.Int( i ) ); }

		bool Uint( unsigned i ) override { return precheck( "unsigned" ) && postcheck( internal.Uint( i ) ); }

		bool Int64( std::int64_t i ) override
		{
			return precheck( "int64_t" ) && postcheck( internal.Int64( i ) );
		}

		bool Uint64( std::uint64_t i ) override
		{
			return precheck( "uint64_t" ) && postcheck( internal.Uint64( i ) );
		}

		bool Double( double d ) override { return precheck( "double" ) && postcheck( internal.Double( d ) ); }

		bool String( const char* str, SizeType length, bool copy ) override
		{
			return precheck( "string" ) && postcheck( internal.String( str, length, copy ) );
		}

		bool Key( const char* str, SizeType length, bool copy ) override
		{
			return precheck( "object" ) && postcheck( internal.Key( str, length, copy ) );
		}

		bool StartObject() override { return precheck( "object" ) && postcheck( internal.StartObject() ); }

		bool EndObject( SizeType length ) override
		{
			return precheck( "object" ) && postcheck( internal.EndObject( length ) );
		}

		bool StartArray() override
		{
			++depth;
			if ( depth > 1 )
				return postcheck( internal.StartArray() );
			return true;
		}

		bool EndArray( SizeType length ) override
		{
			--depth;

			// When depth > 1, this event should be forwarded to the element
			if ( depth > 0 )
				return postcheck( internal.EndArray( length ) );

			this->parsed = true;
			return true;
		}

		bool reap_error( ErrorStack& stk ) override
		{
			if ( !the_error )
				return false;
			stk.push( the_error.release() );
			internal.reap_error( stk );
			return true;
		}

		bool write( IHandler* output ) const override
		{
			if ( !output->StartArray() )
				return false;
			for ( auto&& e : *m_value )
			{
				Handler<ElementType> h( &e );
				if ( !h.write( output ) )
					return false;
			}
			return output->EndArray( static_cast<staticjson::SizeType>( m_value->Num() ) );
		}

		void generate_schema( Value& output, MemoryPoolAllocator& alloc ) const override
		{
			output.SetObject();
			output.AddMember( rapidjson::StringRef( "type" ), rapidjson::StringRef( "array" ), alloc );
			Value items;
			internal.generate_schema( items, alloc );
			output.AddMember( rapidjson::StringRef( "items" ), items, alloc );
		}
	};

	template <typename T>
	class Handler<TArray<T>> : public UnrealArrayHandler<TArray<T>>
	{
	public:

	public:
		explicit Handler( TArray<T>* value ) : UnrealArrayHandler<TArray<T>>( value ) {}

		std::string type_name() const override
		{
			return "TArray<" + this->internal.type_name() + ">";
		}
	};

	//======================================================================//
	//								TMap									//
	//======================================================================//
	template <class MapType>
	class UnrealMapHandler : public BaseHandler
	{
	protected:
		//typedef typename MapType::ElementType ElementType;
		typedef typename MapType::ElementType::ValueType ElementType;
		//typedef typename TRValueToLValueReference<MapType::ElementType::ValueInitType> ElementType;
	protected:
		ElementType element;
		Handler<ElementType> internal_handler;
		MapType* m_value;
		//std::string current_key;
		FString current_key;
		int depth = 0;

	protected:
		void reset() override
		{
			element = ElementType();
			//current_key.clear();
			current_key.Empty();
			internal_handler.prepare_for_reuse();
			depth = 0;
		}

		bool precheck( const char* type )
		{
			if ( depth <= 0 )
			{
				set_type_mismatch( type );
				return false;
			}
			return true;
		}

		bool postcheck( bool success )
		{
			if ( !success )
			{
				std::string key	= TCHAR_TO_UTF8( *current_key );
				the_error.reset( new error::ObjectMemberError( key ) );
			}
			else
			{
				if ( internal_handler.is_parsed() )
				{
					m_value->Emplace( std::move( current_key ), std::move( element ) );
					element = ElementType();
					internal_handler.prepare_for_reuse();
				}
			}
			return success;
		}

	public:
		explicit UnrealMapHandler( MapType* value ) : element(), internal_handler( &element ), m_value( value ) {}

		bool Null() override { return precheck( "null" ) && postcheck( internal_handler.Null() ); }

		bool Bool( bool b ) override { return precheck( "bool" ) && postcheck( internal_handler.Bool( b ) ); }

		bool Int( int i ) override { return precheck( "int" ) && postcheck( internal_handler.Int( i ) ); }

		bool Uint( unsigned i ) override
		{
			return precheck( "unsigned" ) && postcheck( internal_handler.Uint( i ) );
		}

		bool Int64( std::int64_t i ) override
		{
			return precheck( "int64_t" ) && postcheck( internal_handler.Int64( i ) );
		}

		bool Uint64( std::uint64_t i ) override
		{
			return precheck( "uint64_t" ) && postcheck( internal_handler.Uint64( i ) );
		}

		bool Double( double d ) override
		{
			return precheck( "double" ) && postcheck( internal_handler.Double( d ) );
		}

		bool String( const char* str, SizeType length, bool copy ) override
		{
			return precheck( "string" ) && postcheck( internal_handler.String( str, length, copy ) );
		}

		bool Key( const char* str, SizeType length, bool copy ) override
		{
			if ( depth > 1 )
				return postcheck( internal_handler.Key( str, length, copy ) );

			current_key	= FString( str );
			//current_key.assign( str, length );
			return true;
		}

		bool StartArray() override
		{
			return precheck( "array" ) && postcheck( internal_handler.StartArray() );
		}

		bool EndArray( SizeType length ) override
		{
			return precheck( "array" ) && postcheck( internal_handler.EndArray( length ) );
		}

		bool StartObject() override
		{
			++depth;
			if ( depth > 1 )
				return postcheck( internal_handler.StartObject() );
			return true;
		}

		bool EndObject( SizeType length ) override
		{
			--depth;
			if ( depth > 0 )
				return postcheck( internal_handler.EndObject( length ) );
			this->parsed = true;
			return true;
		}

		bool reap_error( ErrorStack& errs ) override
		{
			if ( !this->the_error )
				return false;

			errs.push( this->the_error.release() );
			internal_handler.reap_error( errs );
			return true;
		}

		bool write( IHandler* out ) const override
		{
			if ( !out->StartObject() )
				return false;

			for ( auto pair = ( *m_value ).CreateIterator(); pair; ++pair )
			{
				FString key	= pair.Key();
				const char* keyData = TCHAR_TO_UTF8( *key );
				if ( !out->Key( keyData, static_cast<SizeType>( key.Len() ), true ) )
					return false;
				Handler<ElementType> h( &( pair.Value() ) );
				if ( !h.write( out ) )
					return false;
			}
			return out->EndObject( static_cast<SizeType>( m_value->Num() ) );
		}

		void generate_schema( Value& output, MemoryPoolAllocator& alloc ) const override
		{
			Value internal_schema;
			internal_handler.generate_schema( internal_schema, alloc );
			output.SetObject();
			output.AddMember( rapidjson::StringRef( "type" ), rapidjson::StringRef( "object" ), alloc );

			Value empty_obj( rapidjson::kObjectType );
			output.AddMember( rapidjson::StringRef( "properties" ), empty_obj, alloc );
			output.AddMember( rapidjson::StringRef( "additionalProperties" ), internal_schema, alloc );
		}
	};


	template <class T>
	class Handler<TMap<FString, T>>
		: public UnrealMapHandler<TMap<FString, T>>
	{
	public:
		explicit Handler( TMap<FString, T>* value )
			: UnrealMapHandler<TMap<FString, T>>( value )
		{
		}

		std::string type_name() const override
		{
			return "TMap<FString, " + this->internal_handler.type_name() + ">";
		}
	};

	//======================================================================//
	//								UProperty								//
	//======================================================================//
	template <>
	class Handler<UProperty> : public BaseHandler
	{
	private:
		UProperty* m_value;
		UObject* m_parent;
		int32 m_index;

		UStruct* ownerStruct;
	public:
		explicit Handler( UProperty* v )
			: m_value( v )
			, m_parent( nullptr )
			, m_index( 0 )
		{
			UObject* temp	= m_value->GetOuter();
			while ( nullptr != temp && !temp->IsA( UClass::StaticClass() ) )
			{
				m_parent	= temp;
				temp		= m_parent->GetOuter();
			}
			if ( m_parent->IsA( UClass::StaticClass() ) )
			{
				m_index	= 0;
			}
			m_index		= m_value->GetOffset_ForInternal();

			ownerStruct = m_value->GetOwnerStruct();
		}

		bool Bool( bool value ) override
		{
			if ( m_value->IsA( UBoolProperty::StaticClass() ) )
			{
				UBoolProperty* uProperty	= Cast<UBoolProperty>( m_value );
				UProperty* ownerProp		= m_value->GetOwnerProperty();
				uProperty->SetPropertyValue_InContainer( m_parent, value, m_index );
				this->parsed = true;
				return true;
			}
			else
			{ return set_type_mismatch( "bool" ); }
		}

		bool Int( int value ) override
		{
			if ( m_value->IsA( UIntProperty::StaticClass() ) )
			{
				UIntProperty* uProperty		= Cast<UIntProperty>( m_value );
				UProperty* ownerProp		= m_value->GetOwnerProperty();
				uProperty->SetPropertyValue_InContainer( m_parent, value, m_index );
				this->parsed = true;
				return true;
			}
			else if ( m_value->IsA( UInt16Property::StaticClass() ) )
			{
				UInt16Property* uProperty	= Cast<UInt16Property>( m_value );
				UProperty* ownerProp		= m_value->GetOwnerProperty();
				uProperty->SetPropertyValue_InContainer( m_parent, value, m_index );
				this->parsed = true;
				return true;
			}
			else if ( m_value->IsA( UInt8Property::StaticClass() ) )
			{
				UInt8Property* uProperty	= Cast<UInt8Property>( m_value );
				UProperty* ownerProp		= m_value->GetOwnerProperty();
				uProperty->SetPropertyValue_InContainer( m_parent, value, m_index );
				this->parsed = true;
				return true;
			}
			else
			{ return set_type_mismatch( "int" ); }
		}

		bool Int64( std::int64_t value ) override
		{
			if ( m_value->IsA( UInt64Property::StaticClass() ) )
			{
				UInt64Property* uProperty	= Cast<UInt64Property>( m_value );
				UProperty* ownerProp		= m_value->GetOwnerProperty();
				uProperty->SetPropertyValue_InContainer( m_parent, value, m_index );
				this->parsed = true;
				return true;
			}
			else
			{ return set_type_mismatch( "int64_t" ); }
		}

		bool Double( double value ) override
		{
			if ( m_value->IsA( UDoubleProperty::StaticClass() ) )
			{
				UDoubleProperty* uProperty	= Cast<UDoubleProperty>( m_value );
				UProperty* ownerProp		= m_value->GetOwnerProperty();
				uProperty->SetPropertyValue_InContainer( m_parent, value, m_index );
				this->parsed = true;
				return true;
			}
			else if ( m_value->IsA( UFloatProperty::StaticClass() ) )
			{
				UFloatProperty* uProperty	= Cast<UFloatProperty>( m_value );
				UProperty* ownerProp		= m_value->GetOwnerProperty();
				uProperty->SetPropertyValue_InContainer( m_parent, (float)value, m_index );
				this->parsed = true;
				return true;
			}
			else
			{ return set_type_mismatch( "double" ); }
		}

		bool String( const char* str, SizeType length, bool ) override
		{
			if ( m_value->IsA( UStrProperty::StaticClass() ) )
			{
				UStrProperty* uProperty		= Cast<UStrProperty>( m_value );
				FString strValue( str );

				// method 1 - @ SetPropertyValue_InContainer() : Exception thrown (vcruntime140.dll) in UE4Editor.exe: 0xC0000005: Access violation writing location
				//UProperty* ownerProp		= m_value->GetOwnerProperty();
				//uProperty->SetPropertyValue_InContainer( m_parent, FString( str ), m_index );

				// method 2 - @ ContainerPtrToValuePtr : fails check in UnrealType.h, line 344, check(GetOuter()->IsA(UClass::StaticClass()));
				//void* valuePtr = uProperty->ContainerPtrToValuePtr<void>( m_parent );
				//uProperty->SetPropertyValue( valuePtr, FString( str ) );

				//ownerStruct
				//UStruct* theStruct			= uProperty->GetOwnerStruct();
				//uProperty->ImportText( *strValue, uProperty, 0, m_parent );

				// method 3 - @ ContainerPtrToValuePtr : fails check in UnrealType.h, line 344, check(GetOuter()->IsA(UClass::StaticClass()));
				//void* valuePtr = uProperty->ContainerPtrToValuePtr<void>( ownerStruct );
				//uProperty->SetPropertyValue( valuePtr, FString( str ) );

				UStruct* theStruct			= uProperty->GetOwnerStruct();
				uProperty->ImportText( *strValue, uProperty, 0, ownerStruct );

				this->parsed = true;
				return true;
			}
			else
			{ return set_type_mismatch( "string" ); }
		}

		void getParentAndIndex( UProperty* parent, int32& index )
		{
			parent	= m_value->GetOwnerProperty();
			index	= m_value->GetOffset_ForInternal();
		}

		std::string type_name() const override { return "UProperty"; }

		bool write( IHandler* out ) const override
		{
			if ( m_value->IsA( UStrProperty::StaticClass() ) )
			{
				UStrProperty* uProperty		= Cast<UStrProperty>( m_value );
				FString value				= uProperty->GetPropertyValue_InContainer( m_parent, m_index );
				std::string tempStr( TCHAR_TO_UTF8( *value ) );
				return out->String( tempStr.data(), SizeType( tempStr.size() ), true );
			}
			else if ( m_value->IsA( UInt64Property::StaticClass() ) )
			{
				UInt64Property* uProperty	= Cast<UInt64Property>( m_value );
				int64 value					= uProperty->GetPropertyValue_InContainer( m_parent, m_index );
				return out->Int( value );
			}
			else if ( m_value->IsA( UIntProperty::StaticClass() ) )
			{
				UIntProperty* uProperty	= Cast<UIntProperty>( m_value );
				int value					= uProperty->GetPropertyValue_InContainer( m_parent, m_index );
				return out->Int( value );
			}
			else if ( m_value->IsA( UInt16Property::StaticClass() ) )
			{
				UInt16Property* uProperty	= Cast<UInt16Property>( m_value );
				int value					= uProperty->GetPropertyValue_InContainer( m_parent, m_index );
				return out->Int( value );
			}
			else if ( m_value->IsA( UInt8Property::StaticClass() ) )
			{
				UInt8Property* uProperty	= Cast<UInt8Property>( m_value );
				int64 value					= uProperty->GetPropertyValue_InContainer( m_parent, m_index );
				return out->Int( value );
			}
			else if ( m_value->IsA( UBoolProperty::StaticClass() ) )
			{
				UBoolProperty* uProperty	= Cast<UBoolProperty>( m_value );
				bool value					= uProperty->GetPropertyValue_InContainer( m_parent, m_index );
				return out->Bool( value );
			}
			else if ( m_value->IsA( UDoubleProperty::StaticClass() ) )
			{
				UDoubleProperty* uProperty	= Cast<UDoubleProperty>( m_value );
				double value				= uProperty->GetPropertyValue_InContainer( m_parent, m_index );
				return out->Double( value );
			}
			else if ( m_value->IsA( UFloatProperty::StaticClass() ) )
			{
				UFloatProperty* uProperty	= Cast<UFloatProperty>( m_value );
				double value				= uProperty->GetPropertyValue_InContainer( m_parent, m_index );
				return out->Double( value );
			}
			return false;
		}

		void generate_schema( Value& output, MemoryPoolAllocator& alloc ) const override
		{
			output.SetObject();
			output.AddMember( rapidjson::StringRef( "type" ), rapidjson::StringRef( "UProperty" ), alloc );
		}
	};
}
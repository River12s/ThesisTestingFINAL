#include "ParameterLLMResponse.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

static TArray<TSharedPtr<FJsonValue>> MessagesArray;


void UParameterLLMResponse::UParameterLLMResponseFunction(FString Prompt, FParameterLLMResponse OnResponse,	FParameterLLMError OnError)
{
	// Build the JSON body
	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
	RequestObj->SetStringField(TEXT("model"), TEXT("ParameterSelector:latest"));
	RequestObj->SetBoolField(TEXT("stream"), false);
	RequestObj->SetStringField(TEXT("prompt"), Prompt);

	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer);

	// Create HTTP request
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	Request->SetURL(TEXT("http://localhost:11434/api/generate")); //to ask the AI without memory of previous messages.
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(RequestBody);
	Request->SetActivityTimeout(60);


	FParameterLLMResponse ResponseDelegate = OnResponse;
	FParameterLLMError ResponseErrorDelegate = OnError;
	// Use lambda since we're in a static context
	Request->OnProcessRequestComplete().BindLambda(
		[ResponseDelegate, ResponseErrorDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool  bWasSuccessful)
		{
			UE_LOG(LogTemp, Warning, TEXT("In the thing"));
			if (!bWasSuccessful || !Response.IsValid())
			{
				FString Errormessage = ("An error occured.");
				UE_LOG(LogTemp, Error, TEXT("LLM request failed."));

				if (ResponseErrorDelegate.IsBound())
				{
					ResponseErrorDelegate.Execute(Errormessage);
				}
				return;
			}

			FString ResponseStr = Response->GetContentAsString();

			TSharedPtr<FJsonObject> OuterJsonObj;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);

			if (FJsonSerializer::Deserialize(Reader, OuterJsonObj) && OuterJsonObj.IsValid())
			{
				FString InnerResponseStr;
				if (OuterJsonObj->TryGetStringField(TEXT("response"), InnerResponseStr))
				{
					// Clean the inner "response" string ONLY
					FString Cleaned1 = InnerResponseStr.Replace(TEXT("```"), TEXT(""));
					FString Cleaned = Cleaned1.Replace(TEXT("json"), TEXT(""));
					Cleaned = Cleaned.TrimStartAndEnd();

					// Now parse the cleaned string as JSON
					TSharedPtr<FJsonObject> InnerJsonObj;
					TSharedRef<TJsonReader<>> InnerReader = TJsonReaderFactory<>::Create(Cleaned);

					if (FJsonSerializer::Deserialize(InnerReader, InnerJsonObj) && InnerJsonObj.IsValid())
					{
						FString GenerateStr, Size, Type;

						bool GenerateStrBool = false;
						InnerJsonObj->TryGetStringField(TEXT("Generate"), GenerateStr);
						if (GenerateStr == "true")
						{
							GenerateStrBool = true;
						}
						
						InnerJsonObj->TryGetStringField(TEXT("Size"), Size);
						InnerJsonObj->TryGetStringField(TEXT("Type"), Type);

						UE_LOG(LogTemp, Display, TEXT("Generate: %s"), *GenerateStr);
						UE_LOG(LogTemp, Display, TEXT("bGenerate: %s"), GenerateStrBool ? TEXT("true") : TEXT("false"));
						UE_LOG(LogTemp, Display, TEXT("Size: %s"), *Size);
						UE_LOG(LogTemp, Display, TEXT("Type: %s"), *Type);

						// Call delegate if needed
						if (ResponseDelegate.IsBound())
						{
							ResponseDelegate.Execute(GenerateStrBool, Size, Type); // or pass the values individually
						}
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("Failed to parse inner JSON (parameters)"));
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("Outer JSON has no 'response' field."));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to parse outer JSON"));
			}
		}
	);


	Request->ProcessRequest();
}

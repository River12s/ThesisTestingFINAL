
#include "LLMResponseComplete.h"
#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Json.h"
#include "JsonUtilities.h"

static TArray<TSharedPtr<FJsonValue>> MessagesArray;


void ULLMResponseComplete::ULLMCallerfunctionComplete(FString Prompt, FOnLLMResponseComplete OnResponse)
{

	//FHttpModule::Get().Set
	
	// Build the JSON body
	TSharedPtr<FJsonObject> RequestObj = MakeShareable(new FJsonObject);
	//RequestObj->SetStringField(TEXT("model"), TEXT("gemma3:4b"));
	RequestObj->SetStringField(TEXT("model"), TEXT("hf.co/mradermacher/JSL-Med-Mistral-24B-V1-Slerp-i1-GGUF:latest"));
	//RequestObj->SetStringField(TEXT("prompt"), Prompt);
	RequestObj->SetBoolField(TEXT("stream"), false);
	//RequestObj->SetNumberField(TEXT("request_timeout"), 600);
	//RequestObj->SetNumberField(TEXT("keep_alive"), 60000);

	//Use a chat completion instead of a prompt
	TSharedPtr<FJsonObject> MessageObj = MakeShareable(new FJsonObject);
	MessageObj->SetStringField(TEXT("role"), "user");
	MessageObj->SetStringField(TEXT("content"), Prompt);
	
	//Printing the MessageObj for testing
	FString OutputString1;
	TSharedRef<TJsonWriter<>> Writer1 = TJsonWriterFactory<>::Create(&OutputString1);
	FJsonSerializer::Serialize(MessageObj.ToSharedRef(), Writer1);
	UE_LOG(LogTemp, Log, TEXT("MessageObj: %s"), *OutputString1);

	//Wrap in Json and make it into an array
	//TSharedPtr<FJsonValueObject> MessageValue = MakeShareable(new FJsonValueObject(MessageObj));
	//TArray<TSharedPtr<FJsonValue>> MessagesArray;
	MessagesArray.Add(MakeShareable(new FJsonValueObject(MessageObj)));
	RequestObj->SetArrayField(TEXT("messages"), MessagesArray);

	
	FString RequestBody;
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
	FJsonSerializer::Serialize(RequestObj.ToSharedRef(), Writer);

	// Create HTTP request
	TSharedRef<IHttpRequest> Request = FHttpModule::Get().CreateRequest();
	//Request->SetURL(TEXT("http://localhost:11434/api/generate")); //to ask the AI without memory of previous messages.
	Request->SetURL(TEXT("http://localhost:11434/api/chat")); //includes memory
	//Request->SetURL(TEXT("http://127.0.0.1:11434/api/chat"));
	Request->SetVerb(TEXT("POST"));
	Request->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	Request->SetContentAsString(RequestBody);
	Request->SetActivityTimeout(60);


	FOnLLMResponseComplete ResponseDelegate = OnResponse;
	// Use lambda since we're in a static context
	Request->OnProcessRequestComplete().BindLambda(
		 [ResponseDelegate](FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
		{
		 	UE_LOG(LogTemp, Warning ,TEXT("In the thing"));
			if (!bWasSuccessful || !Response.IsValid())
			{
				FString Error = ("An error occured.");
				UE_LOG(LogTemp, Error, TEXT("LLM request failed."));
				
				if (ResponseDelegate.IsBound())
				{
					ResponseDelegate.Execute(Error);
				}
				return;
			}

			FString ResponseStr = Response->GetContentAsString();
		 	//UE_LOG(LogTemp, Log, TEXT("ResponseStr: %s"), *ResponseStr);

			TSharedPtr<FJsonObject> JsonObj;
			TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(ResponseStr);
			if (FJsonSerializer::Deserialize(Reader, JsonObj) && JsonObj.IsValid())
			{
				TSharedPtr<FJsonObject> MessageObj2 = JsonObj->GetObjectField(TEXT("message"));
				//FString Output = JsonObj->GetStringField(TEXT("content"));
				
				//for (const TSharedPtr<FJsonValue>& Value : Message)
				// {
				// 	TSharedPtr<FJsonObject> MessageObj = Value->AsObject();
				// 	if (MessageObj.IsValid())
				// 	{
						FString Role = MessageObj2->GetStringField(TEXT("role"));
						FString Content = MessageObj2->GetStringField(TEXT("content"));

						UE_LOG(LogTemp, Display, TEXT("Role: %s"), *Role);
						UE_LOG(LogTemp, Display, TEXT("LLM Response: %s"), *Content);

						TSharedPtr<FJsonObject> AssistantMessageObj = MakeShareable(new FJsonObject);
						AssistantMessageObj->SetStringField(TEXT("role"), Role);
						AssistantMessageObj->SetStringField(TEXT("content"), Content);
						MessagesArray.Add(MakeShareable(new FJsonValueObject(AssistantMessageObj)));

						//for printing to console
						TSharedPtr<FJsonObject> TempRoot = MakeShareable(new FJsonObject);
						TempRoot->SetArrayField(TEXT("messages"), MessagesArray);
						FString OutputString;
						TSharedRef<TJsonWriter<>> Writer2 = TJsonWriterFactory<>::Create(&OutputString);
						if (FJsonSerializer::Serialize(TempRoot.ToSharedRef(), Writer2))
						{
							UE_LOG(LogTemp, Log, TEXT("MessagesArray JSON:\n%s"), *OutputString);
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("Failed to serialize MessagesArray."));
						}

						//for delegate 
						if (ResponseDelegate.IsBound())
						{
							ResponseDelegate.Execute(Content);
						}
				//	}
				//}
				
			}
		 	else
		 	{
		 		UE_LOG(LogTemp, Error, TEXT("Failed to deserialize response"));
		 	}
		}
	);

	//UE_LOG(LogTemp, Warning, TEXT("Timeout is set to: %f"), Request->GetTimeout());

	Request->ProcessRequest();
}
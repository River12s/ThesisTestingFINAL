// #include "OLLamaStreamReader.h"
//
// #include <string>
// #include <curl/curl.h>
// #include "Json.h"
//
// struct FWriteContext
// {
// 	FOnLLMChunkReceived OnChunkReceived;
// 	FString Buffer;
// };
//
// void UOLLamaStreamReader::GeneratePrompt(const FString& Prompt, FOnLLMChunkReceived OnChunkReceived)
// {
// 	CURL* Curl = curl_easy_init();
// 	if (!Curl)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("Failed to init curl"));
// 		return;
// 	}
//
// 	FWriteContext* Context = new FWriteContext();
// 	Context->OnChunkReceived = OnChunkReceived;
//
// 	FString JsonBody = FString::Printf(TEXT("{\"model\":\"gemma3:4b\",\"prompt\":\"%s\",\"stream\":true}"), *Prompt);
// 	std::string BodyStr(TCHAR_TO_UTF8(*JsonBody));
//
// 	struct curl_slist* Headers = nullptr;
// 	Headers = curl_slist_append(Headers, "Content-Type: application/json");
//
// 	curl_easy_setopt(Curl, CURLOPT_URL, "http://localhost:11434/api/generate");
// 	curl_easy_setopt(Curl, CURLOPT_POST, 1L);
// 	curl_easy_setopt(Curl, CURLOPT_HTTPHEADER, Headers);
// 	curl_easy_setopt(Curl, CURLOPT_POSTFIELDS, BodyStr.c_str());
//
// 	// Set callback
// 	curl_easy_setopt(Curl, CURLOPT_WRITEFUNCTION, CurlWriteCallback);
// 	curl_easy_setopt(Curl, CURLOPT_WRITEDATA, Context);
//
// 	// Enable chunked response
// 	curl_easy_setopt(Curl, CURLOPT_NOPROGRESS, 1L);
// 	curl_easy_setopt(Curl, CURLOPT_TCP_KEEPALIVE, 1L);
//
// 	CURLcode Result = curl_easy_perform(Curl);
// 	if (Result != CURLE_OK)
// 	{
// 		UE_LOG(LogTemp, Error, TEXT("curl_easy_perform() failed: %s"), UTF8_TO_TCHAR(curl_easy_strerror(Result)));
// 	}
//
// 	curl_slist_free_all(Headers);
// 	curl_easy_cleanup(Curl);
// 	delete Context;
// }
//
// size_t UOLLamaStreamReader::CurlWriteCallback(char* ptr, size_t size, size_t nmemb, void* userdata)
// {
// 	size_t TotalSize = size * nmemb;
// 	FWriteContext* Context = static_cast<FWriteContext*>(userdata);
// 	FString ChunkStr = FString(UTF8_TO_TCHAR(ptr)).Left(TotalSize);
//
// 	// May contain multiple lines if data was buffered
// 	TArray<FString> Lines;
// 	ChunkStr.ParseIntoArrayLines(Lines);
//
// 	for (const FString& Line : Lines)
// 	{
// 		if (Line.TrimStartAndEnd().IsEmpty())
// 			continue;
//
// 		TSharedPtr<FJsonObject> JsonObj;
// 		TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Line);
// 		if (FJsonSerializer::Deserialize(Reader, JsonObj) && JsonObj.IsValid())
// 		{
// 			const FString* Response = nullptr;
// 			FString ResponseText;
// 			if (JsonObj->TryGetStringField(TEXT("response"), ResponseText ))
// 			{
// 				Context->OnChunkReceived.ExecuteIfBound(*Response);
// 			}
// 		}
// 		else
// 		{
// 			UE_LOG(LogTemp, Warning, TEXT("Failed to parse streamed chunk: %s"), *Line);
// 		}
// 	}
//
// 	return TotalSize;
// }

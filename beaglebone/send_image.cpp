#include <iostream>
#include <curl/curl.h>

int main(){
	CURL *curl;
	CURLcode res;

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if (curl) {
		// Set the URL of the Flask API endpoint
		curl_easy_setopt(curl, CURLOPT_URL, "https://rnnbw-128-197-29-224.a.free.pinggy.link/upload");
		// Set the HTTP POST method
		curl_easy_setopt(curl, CURLOPT_POST, 1L);
		// Set the form file to upload
		curl_httppost *formpost = NULL;
		curl_httppost *lastptr = NULL;
		curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "file",
		                  CURLFORM_FILE, "image.png", CURLFORM_END);
		// Set the form content type
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		// Set the form data
	        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
		// Perform the HTTP request
		res = curl_easy_perform(curl);
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
		    	curl_easy_strerror(res));
		}
		curl_easy_cleanup(curl);
		curl_formfree(formpost);
		curl_slist_free_all(headers);
	}
	curl_global_cleanup();
	return 0;
}

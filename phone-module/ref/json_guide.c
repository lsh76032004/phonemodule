#include <string.h>
#include <stdio.h>


#include "parson.h"


/*
	input : full packet string(json format)
	sample : {"key1","value1" : "key2","value2"}
*/
void read_json(char* json_packet)
{
	/*Init json object*/
	JSON_Value *rootValue;
	JSON_Object *rootObject;

	rootValue = json_parse_string(json_packet);
	rootObject = json_value_get_object(rootValue);

	/*Get value function*/
	printf("[key : %s] [data : %s]\n", "key1", json_object_get_string(rootObject, "key1"));
	printf("[key : %s] [data : %s]\n", "key2", json_object_get_string(rootObject, "key2"));

	printf("[key : %s] [data : %d]\n", "key3", (int)json_object_get_number(rootObject, "key3"));

	/*free memory*/
	json_value_free(rootValue);

	/* APPENDIX
	//Get array value 
	int i;
	JSON_Array *array = json_object_get_array(rootObject, "key#");
	for(i=0; i<json_array_get_count(array); i++)
	{
		printf("%s\n", json_array_get_string(array, i);	
	}
	*/
}

/*
	make json packet & print
 */

void write_json()
{
	JSON_Value *rootValue;
	JSON_Object *rootObject;
	char* buf;

	/*init empty json packet*/
	rootValue = json_value_init_object();
	rootObject = json_value_get_object(rootValue);

	/*add key & value*/
	json_object_set_string(rootObject, "key1", "value1");
	json_object_set_string(rootObject, "key2", "value2");
	json_object_set_number(rootObject, "key3", 3);

	/*get full string of json packet */
	buf =  json_serialize_to_string(rootValue);
	printf("result json : %s\n", buf);

	//free memory
	json_free_serialized_string(buf);
	json_value_free(rootValue);
}

int main()
{
	char buf[512];

	while (fgets(buf, sizeof buf, stdin))
	{
		int len = strlen(buf);
		buf[len - 1] = '\0';

		printf("JSON Packet : %s\n", buf);

		read_json(buf);
	}

	write_json();

	return 0;

}


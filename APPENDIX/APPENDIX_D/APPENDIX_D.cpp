/*
*  By Alexandros Panagiotakopoulos
AM:1716/ OLD AM:16108
UNIVERSITY OF IOANNINA - DEPARTMENT OF INFORMATICS & TELECOMMUNICATIONS
*/

// This work is based upon the pioneering work provided by Microsoft
// on their project on the Azure_IoT_PnP Template, which is also part of their ESPRESSIF32 Azure IoT Kit 
// and Azure SDK For C library packages
//
// This Project is an original work created by Alexandros Panagiotakopoulos during the academic year 2021-2022
// and is also part of the Thesis "IoT: Standards & Platforms".

// SOURCES USED:
// AZURE SDK LIBRARY FOR C, https://www.arduino.cc/reference/en/libraries/azure-sdk-for-c/
// DHT11/22 Library Built-In examples, https://github.com/adafruit/DHT-sensor-library


//include all the necessary library files in order to function as intended
#include <stdlib.h> //include the stdlib.h library
#include <stdarg.h> //include the stdarg.h library
#include "DHT.h" //include the DHT.h library
#include <az_core.h> //include the az_core library
#include <az_iot.h> //include the az_iot.h library

#include "AzureIoT.h" //include the Azure IoT.h library
#include "Azure_IoT_PnP_Template.h" //include the Azure_IoT_PnP_Template header file

#include <az_precondition_internal.h> //include the az_precondition_internal header file



//Global Declaration - Definition variables
#define AZURE_PNP_MODEL_ID "dtmi:azureiot:devkit:freertos:Esp32AzureIotKit;1"

//Useful information about the microcontroller which will be displayed into the Azure IoT Central
#define DEVICE_INFORMATION_NAME                 "ESP32 DEVELOPMENT KIT" //declare information about the board's model
#define MANUFACTURER_PROPERTY_NAME              "ESPRESSIF" //declare the manfacturer's name
#define MODEL_PROPERTY_NAME                     "Development Kit" //declare the the board's model
#define MANUFACTURER_PROPERTY_VALUE             "ESPRESSIF" //declare the information about the manufacturer
#define MODEL_PROPERTY_VALUE                    "ESP32 Development Kit" //declare the model property


//delcare the sensor's values which will be sent into the Azure IoT Central
#define TELEMETRY_TEMPERATURE                "temperature" //declare the sensor's temperature
#define TELEMETRY_HUMIDITY                   "humidity" //declare the sensor's humidity
#define WRITABLE_PROPERTY_TELEMETRY_FREQ_SECS          "telemetryFrequencySecs" //declare the telemetry frquency which will be sent into the Azure IoT Central.
#define WRITABLE_PROPERTY_RESPONSE_SUCCESS             "success" //declare the success value if the telemetry succeeds the value transmission
#define DOUBLE_DECIMAL_PLACE_DIGITS 2 //place digits to display the information which will be transmitted

//--------------------------Initializing the DHT11 sensor----------------------------------------
#define DHTPIN 4     // Declare the DHTPIN on PIN 4
#define DHTTYPE DHT22   // Declare the DHT model that is used -- in this case the DHT11

DHT dht(DHTPIN, DHTTYPE); //declare the DHT11 sensor depending on the model



//----------------------------------------------------------------------------------------------

// --- Function Checks and Returns in case everything worked as intened ----------------------- 
#define RESULT_OK       0
#define RESULT_ERROR    __LINE__

#define EXIT_IF_TRUE(condition, retcode, message, ...)                              \
  do                                                                                \
  {                                                                                 \
    if (condition)                                                                  \
    {                                                                               \
      LogError(message, ##__VA_ARGS__ );                                            \
      return retcode;                                                               \
    }                                                                               \
  } while (0)

#define EXIT_IF_AZ_FAILED(azresult, retcode, message, ...)                                   \
  EXIT_IF_TRUE(az_result_failed(azresult), retcode, message, ##__VA_ARGS__ )

// --- end of error checking

#define DATA_BUFFER_SIZE 1024 // --- declare the data buffer size 



static uint8_t data_buffer[DATA_BUFFER_SIZE]; // --- declare the data buffer size 
static uint32_t telemetry_send_count = 0; //telemtry sending counter
static size_t telemetry_frequency_in_seconds = 10; // With default frequency of once in 10 seconds.
static time_t last_telemetry_send_time = INDEFINITE_TIME; //find the last telemetry time that something was sent



//--- Function telemetries that are available into transmitting the JSON messages
static int generate_telemetry_payload( //functions in order to generate the telemetry
  uint8_t* payload_buffer, size_t payload_buffer_size, size_t* payload_buffer_length); //functions to initialize the buffer
static int generate_device_info_payload( //functions to generate information payloard
  az_iot_hub_client const* hub_client, uint8_t* payload_buffer, 
  size_t payload_buffer_size, size_t* payload_buffer_length);

static int consume_properties_and_generate_response( //function to generate response through the Azure IoT Central server
  azure_iot_t* azure_iot, az_span properties,
  uint8_t* buffer, size_t buffer_size, size_t* response_length);



// --- function to generate the pnp payloard -------
void azure_pnp_init()
{
}

//function the generate the pnp model information
const az_span azure_pnp_get_model_id()
{
  return AZ_SPAN_FROM_STR(AZURE_PNP_MODEL_ID);
}

//function that can calculate the Azure IoT Central telemetry in seconds
void azure_pnp_set_telemetry_frequency(size_t frequency_in_seconds)
{
  telemetry_frequency_in_seconds = frequency_in_seconds;
  LogInfo("Telemetry frequency set to once every %d seconds.", telemetry_frequency_in_seconds);
}


// Code to generate telemetry and transmit it throught the Azure IoT Central 
int azure_pnp_send_telemetry(azure_iot_t* azure_iot)
{
  _az_PRECONDITION_NOT_NULL(azure_iot);
  time_t now = time(NULL); //get the current system time
  if (now == INDEFINITE_TIME) //check if the current time can be received
  {
    LogError("Failed getting current time for controlling telemetry."); //if not, print the appropriate time message
    return RESULT_ERROR; //return the error
  }
  else if (last_telemetry_send_time == INDEFINITE_TIME || //else, get the right time
           difftime(now, last_telemetry_send_time) >= telemetry_frequency_in_seconds)
  {
    size_t payload_size; //declare the payload size as a global type variable (size_t)

    last_telemetry_send_time = now; //get the latest telemetry and store it into the variable last_telemtry

    if (generate_telemetry_payload(data_buffer, DATA_BUFFER_SIZE, &payload_size) != RESULT_OK) //check if the buffer telemetry payload can be generated
    {
      LogError("Failed generating telemetry payload."); //if the telemetry is unable to be generated
      return RESULT_ERROR; //then return an error code
    }

    if (azure_iot_send_telemetry(azure_iot, az_span_create(data_buffer, payload_size)) != 0) //if the azure iot telemetry can be generated
    {
      LogError("Failed sending telemetry."); //if the telemetry is unable to be generated
      return RESULT_ERROR; //then return an error code
    }
  }

  return RESULT_OK; //if the everything succeeded without any issues return ok
}

//Code in order to transmit Azure IoT Central Information
int azure_pnp_send_device_info(azure_iot_t* azure_iot, uint32_t request_id)
{
  _az_PRECONDITION_NOT_NULL(azure_iot);

  int result; //declare an integer result value
  size_t length; //declare a global (size_t) length
    
  result = generate_device_info_payload(&azure_iot->iot_hub_client, data_buffer, DATA_BUFFER_SIZE, &length); //check if the telemetry payload can be generated
  EXIT_IF_TRUE(result != RESULT_OK, RESULT_ERROR, "Failed generating telemetry payload."); //if not, print the appropriate message

  result = azure_iot_send_properties_update(azure_iot, request_id, az_span_create(data_buffer, length)); //check if the data buffer (reported properties) can be sent
  EXIT_IF_TRUE(result != RESULT_OK, RESULT_ERROR, "Failed sending reported properties update.");  //if not, print the appropriate message

  return RESULT_OK; //if everything procceeded without any errors then return an "ok" message
}

int azure_pnp_handle_command_request(azure_iot_t* azure_iot, command_request_t command)
{
  _az_PRECONDITION_NOT_NULL(azure_iot);

  uint16_t response_code;

    LogError("Command not recognized (%.*s).", az_span_size(command.command_name), az_span_ptr(command.command_name));

  return azure_iot_send_command_response(azure_iot, command.request_id, response_code, AZ_SPAN_EMPTY);
}

int azure_pnp_handle_properties_update(azure_iot_t* azure_iot, az_span properties, uint32_t request_id) //declare necessary pnp properties and update them
{
  _az_PRECONDITION_NOT_NULL(azure_iot); 
  _az_PRECONDITION_VALID_SPAN(properties, 1, false);

  int result;
  size_t length;

  result = consume_properties_and_generate_response(azure_iot, properties, data_buffer, DATA_BUFFER_SIZE, &length); //check if a response can be generated
  EXIT_IF_TRUE(result != RESULT_OK, RESULT_ERROR, "Failed generating properties ack payload."); //check if any issues occured and print the appropriate message

  result = azure_iot_send_properties_update(azure_iot, request_id, az_span_create(data_buffer, length)); //check if a response can be generated
  EXIT_IF_TRUE(result != RESULT_OK, RESULT_ERROR, "Failed sending reported properties update."); //check if any issues occured and print the appropriate message

  return RESULT_OK;
}

// Code to get the DHT11 Sensor's temperature
float get_temperature()
{
  float t = dht.readTemperature();
  if (isnan(t)) {
    return -30;
  }  
  return t; //send the t variable into the Azure IoT Central when the function is called
}

// Code to get the DHT11 Sensor's humidity
float get_humidity() //declare a float h variable which will store the DHT humidity (relative humidity)
{
  float h = dht.readHumidity();
  if (isnan(h)) {
    return -30;
  }  
  return h; //send the t variable into the Azure IoT Central when the function is called
}

//Code in order to generate the telemetry payloard
static int generate_telemetry_payload(uint8_t* payload_buffer, size_t payload_buffer_size, size_t* payload_buffer_length)
{
  az_json_writer jw; //declare the JSON writter object
  az_result rc; //declare the JSON result
  az_span payload_buffer_span = az_span_create(payload_buffer, payload_buffer_size); //declare a JSON buffer that includes the payload buffer and the size
  az_span json_span; //declare the json span
  float temperature, humidity; //declare the DHT11 temperature & humidity


  // Acquiring the temperature& humidity from the DHT11 sensor
  dht.begin();
  temperature = get_temperature(); //call the function to get the temperature
  humidity = get_humidity(); //call the function to get the humidity

  rc = az_json_writer_init(&jw, payload_buffer_span, NULL);
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed initializing json writer for telemetry.");

  rc = az_json_writer_append_begin_object(&jw);
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed setting telemetry json root.");
  
//--------CODE TO TRANSMIT THE TEMPERATURE TO THE AZURE IOT CENTAL-----------------------------------------------------
  rc = az_json_writer_append_property_name(&jw, AZ_SPAN_FROM_STR(TELEMETRY_TEMPERATURE)); //declare the  temperature through the use of telemetry
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed adding temperature property name to telemetry payload."); //if the transmittion fails print a failure message
  rc = az_json_writer_append_double(&jw, temperature, DOUBLE_DECIMAL_PLACE_DIGITS); //place the appropriate digits into the temperature
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed adding temperature property value to telemetry payload. "); //if the transmittion fails print a failure message

//--------CODE TO TRANSMIT THE HUMIDITY TO THE AZURE IOT CENTAL--------------------------------------------------------
  rc = az_json_writer_append_property_name(&jw, AZ_SPAN_FROM_STR(TELEMETRY_HUMIDITY)); //declare the humidity through the use of telemetry
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed adding himidity property name to telemetry payload."); //if the transmittion fails print a failure message
  rc = az_json_writer_append_double(&jw, humidity, DOUBLE_DECIMAL_PLACE_DIGITS); //place the appropriate digits into the temperature
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed adding humidity property value to telemetry payload. "); //if the transmittion fails print a failure message

  payload_buffer_span = az_json_writer_get_bytes_used_in_destination(&jw); //write the json messages into the buffer

  if ((payload_buffer_size - az_span_size(payload_buffer_span)) < 1) //check if there is an efficient buffer size to generate the telemetry
  {
    LogError("Insufficient space for telemetry payload null terminator."); //print an error message in case the telemetry is inefficient
    return RESULT_ERROR; //return the error message
  }

  payload_buffer[az_span_size(payload_buffer_span)] = null_terminator; //give the payload buffer null terminator value
  *payload_buffer_length = az_span_size(payload_buffer_span); //store the payload buffer span into the payload buffer value if there werent any issues
 
  return RESULT_OK; //return an ok result
}

static int generate_device_info_payload(az_iot_hub_client const* hub_client, uint8_t* payload_buffer, size_t payload_buffer_size, size_t* payload_buffer_length)
{
  az_json_writer jw; //declare the JSON writter object
  az_result rc; //declare the JSON result
  az_span payload_buffer_span = az_span_create(payload_buffer, payload_buffer_size); //declare the json span
  az_span json_span; //call the function to get the humidity

  rc = az_json_writer_init(&jw, payload_buffer_span, NULL); //check if the payload buffer size is available
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed initializing json writer for telemetry.");  //if not, print the appropriate message

  rc = az_json_writer_append_begin_object(&jw); //check if the root object writing is available
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed setting telemetry json root.");  //if not, print the appropriate message
  
  rc = az_iot_hub_client_properties_writer_begin_component( //check if Azure IoT Hub client properties writter is available
    hub_client, &jw, AZ_SPAN_FROM_STR(DEVICE_INFORMATION_NAME)); //if it is available then print the device information name into the platform
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed writting component name.");  //if not, print the appropriate message

  //add the manufacturer name to the JSON payload and check if there are any issues
  rc = az_json_writer_append_property_name(&jw, AZ_SPAN_FROM_STR(MANUFACTURER_PROPERTY_NAME)); //check if the manufacturer's property name can be transmitted
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed adding MANUFACTURER_PROPERTY_NAME to payload."); //if an error occured return the appropriate error message
  rc = az_json_writer_append_string(&jw, AZ_SPAN_FROM_STR(MANUFACTURER_PROPERTY_VALUE)); //check if the manufacturer's property name can be transmitted
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed adding MANUFACTURER_PROPERTY_VALUE to payload. "); //if an error occured return the appropriate error message

    //add the property name to the JSON payload and check if there are any issues
  rc = az_json_writer_append_property_name(&jw, AZ_SPAN_FROM_STR(MODEL_PROPERTY_NAME)); //check if the model's property name can be transmitted
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed adding MODEL_PROPERTY_NAME to payload."); //if an error occured return the appropriate error message
  rc = az_json_writer_append_string(&jw, AZ_SPAN_FROM_STR(MODEL_PROPERTY_VALUE));//check if the model's property name can be transmitted
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed adding MODEL_PROPERTY_VALUE to payload. "); //if an error occured return the appropriate error message


  rc = az_iot_hub_client_properties_writer_end_component(hub_client, &jw);  //check the hub is available to write components and settings
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed closing component object.");  //if an error occured return the appropriate error message

  rc = az_json_writer_append_end_object(&jw); //check if the JSON messages are available to write
  EXIT_IF_AZ_FAILED(rc, RESULT_ERROR, "Failed closing telemetry json payload."); //if an error occured return the appropriate error message

  payload_buffer_span = az_json_writer_get_bytes_used_in_destination(&jw); //write JSON byte messages and store them in the payload buffer

  if ((payload_buffer_size - az_span_size(payload_buffer_span)) < 1) //check if the payload size is not enough
  {
    LogError("Insufficient space for telemetry payload null terminator.");  //if an error occured return the appropriate error message
    return RESULT_ERROR; //return the reason as to why this error occured
  }

  payload_buffer[az_span_size(payload_buffer_span)] = null_terminator; //give the payload buffer null terminator value
  *payload_buffer_length = az_span_size(payload_buffer_span); //store the payload buffer span into the payload buffer value if there werent any issues
 
  return RESULT_OK; //if everything proceeded without any issues then procceed
}

static int generate_properties_update_response( //in the following lines check if the JSON writer is initialized and update a response
  azure_iot_t* azure_iot, 
  az_span component_name, int32_t frequency, int32_t version,
  uint8_t* buffer, size_t buffer_size, size_t* response_length)
{
  az_result azrc;
  az_json_writer jw;
  az_span response = az_span_create(buffer, buffer_size);

  azrc = az_json_writer_init(&jw, response, NULL); //check if the JSON writter is writtable or not
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed initializing json writer for properties update response."); //if not, then print the appropriate message

  azrc = az_json_writer_append_begin_object(&jw); //check if the JSON in properties is available
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed opening json in properties update response."); //if not, then print the appropriate message

  // This Azure PnP Template does not have a named component,
  // so az_iot_hub_client_properties_writer_begin_component is not needed.

  azrc = az_iot_hub_client_properties_writer_begin_response_status( //check if the azure iot hub is writtable and sends a response
    &azure_iot->iot_hub_client,
    &jw,
    AZ_SPAN_FROM_STR(WRITABLE_PROPERTY_TELEMETRY_FREQ_SECS),
    (int32_t)AZ_IOT_STATUS_OK,
    version,
    AZ_SPAN_FROM_STR(WRITABLE_PROPERTY_RESPONSE_SUCCESS)); //check if the IoT hub has responded
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed appending status to properties update response."); //return the appropriate failure message

  azrc = az_json_writer_append_int32(&jw, frequency); //declare the frequency and store it into the AZRC variable
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed appending frequency value to properties update response."); //if there were any errors reading the frequency then return the appropriate message

  azrc = az_iot_hub_client_properties_writer_end_response_status(&azure_iot->iot_hub_client, &jw); //initialize the azure JSON response value
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed closing status section in properties update response."); //check if any issues occured and print the appropriate message


  azrc = az_json_writer_append_end_object(&jw); //initialize the azure JSON writer append end object
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed closing json in properties update response."); //check if any issues occured and print the appropriate message

  *response_length = az_span_size(az_json_writer_get_bytes_used_in_destination(&jw)); //write the JSON span size

  return RESULT_OK; //if everything procceeded without any errors then return ok
}

static int consume_properties_and_generate_response(
  azure_iot_t* azure_iot, az_span properties,
  uint8_t* buffer, size_t buffer_size, size_t* response_length)
{
  int result;
  az_json_reader jr;
  az_span component_name;
  int32_t version = 0;

  az_result azrc = az_json_reader_init(&jr, properties, NULL);
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed initializing json reader for properties update."); //check if any issues occured and print the appropriate message

  const az_iot_hub_client_properties_message_type message_type =
    AZ_IOT_HUB_CLIENT_PROPERTIES_MESSAGE_TYPE_WRITABLE_UPDATED;

  azrc = az_iot_hub_client_properties_get_properties_version(
    &azure_iot->iot_hub_client, &jr, message_type, &version);
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed writable properties version."); //check if any issues occured and print the appropriate message

  azrc = az_json_reader_init(&jr, properties, NULL);
  EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed re-initializing json reader for properties update."); //check if any issues occured and print the appropriate message

  while (az_result_succeeded(
    azrc = az_iot_hub_client_properties_get_next_component_property(
      &azure_iot->iot_hub_client, &jr, message_type,
      AZ_IOT_HUB_CLIENT_PROPERTY_WRITABLE, &component_name)))
  {
    if (az_json_token_is_text_equal(&jr.token, AZ_SPAN_FROM_STR(WRITABLE_PROPERTY_TELEMETRY_FREQ_SECS)))
    {
      int32_t value;
      azrc = az_json_reader_next_token(&jr); //check if the JSON reader next token can be written
      EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed getting writable properties next token.");  //if not, print the appropriate message

      azrc = az_json_token_get_int32(&jr.token, &value); //check if the JSON reader next token can be written
      EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed getting writable properties int32_t value.");  //if not, print the appropriate message

      azure_pnp_set_telemetry_frequency((size_t)value); //apply the appropriate telemetry frequency

      result = generate_properties_update_response(
        azure_iot, component_name, value, version, buffer, buffer_size, response_length);
      EXIT_IF_TRUE(result != RESULT_OK, RESULT_ERROR, "generate_properties_update_response failed.");  //if not, print the appropriate message
    }
    else
    {
      LogError("Unexpected property received (%.*s).", //check if any unexpected properties were received
        az_span_size(jr.token.slice), az_span_ptr(jr.token.slice));
    }

    azrc = az_json_reader_next_token(&jr); //check if the JSON next token is available
    EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed moving to next json token of writable properties.");  //if not, print the appropriate message

    azrc = az_json_reader_skip_children(&jr); //check if the JSON reader skip children is available
    EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed skipping children of writable properties.");  //if not, print the appropriate message

    azrc = az_json_reader_next_token(&jr); //check if JSON reader next token can be written
    EXIT_IF_AZ_FAILED(azrc, RESULT_ERROR, "Failed moving to next json token of writable properties (again)."); //if not, print the appropriate message
  }

  return RESULT_OK; //if everything procceeded without any errors return the appropriate message
}

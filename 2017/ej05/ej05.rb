#!/usr/bin/ruby
require 'openssl'
require 'net/http'
require 'base64'

WINDOW_SIZE=12
BYTES=3445
message=""
uri = URI('https://52.49.91.111:8443/ghost')

def download_data(uri)
  start=0
  while start < BYTES
    last = [start + WINDOW_SIZE - 1, BYTES].min
    puts "Requesting from #{start} to #{last}"
    Net::HTTP.start(uri.host, uri.port,
      use_ssl: true, verify_mode: OpenSSL::SSL::VERIFY_NONE) do |http|
      request = Net::HTTP::Get.new uri
      request['Range'] = "bytes=#{start}-#{last}"

      response = http.request request # Net::HTTPResponse object
      message << response.body
    end
    start+= WINDOW_SIZE
  end

  File.write('message', message)
end

def decode_data
  File.write('decoded_message', Base64.decode64(File.read('message')))
end

# REST API Showcase
This project is just to showcase interchangable backend of different languages i have experience in. We build the same simply REST API connecting to a mysql database.


## Helpful commands
### mysql
```sh
docker compose -f db-compose.yaml pull
docker stack deploy -c db-compose.yaml REST
```
### cpp
```sh
docker build ./cpp -f ./cpp/Dockerfile -t cpp_rest_api
docker stack deploy -c ./cpp/cpp-rest-api-compose.yaml REST
```
### rust
```sh
docker build ./rust -f ./rust/Dockerfile -t rust_rest_api
docker stack deploy -c ./rust/rust-rest-api-compose.yaml REST
```
### javascript
```sh
docker build ./javascript -f ./javascript/Dockerfile -t js_rest_api
docker stack deploy -c ./javascript/js-rest-api-compose.yaml REST
```
### curl
```sh
# get all
curl -v http://localhost:5000/videos
# get
curl -4 -v -X GET http://127.0.0.1:5000/video/1
# put
curl -4 -v -X PUT -H "Content-Type: application/json" -d '{"name":"test","likes":1,"views":10}' http://127.0.0.1:5000/video/3
# post
curl -4 -v -X POST -H "Content-Type: application/json" -d '{"views":200}' http://127.0.0.1:5000/video/1
# delete
curl -4 -v -X DELETE http://127.0.0.1:5000/video/3
```


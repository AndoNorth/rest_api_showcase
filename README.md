# REST API Showcase
This project is just to showcase interchangable backend of different languages i have experience in. We build the same simply REST API connecting to a mysql database.


## Helpful commands
### mysql
```sh
# db
docker compose -f db-compose pull
docker stack deploy -c db-compose REST
```
### cpp
```sh
docker build ./cpp -f ./cpp/Dockerfile -t cpp_rest_api
docker stack deploy -c ./cpp/cpp-rest-api-compose.yaml REST
```


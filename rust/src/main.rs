mod app;
mod db;
mod handlers;
mod models;

use db::mysql::create_pool;

#[tokio::main]
async fn main() -> Result<(), Box<dyn std::error::Error>> {
    tracing_subscriber::fmt::init();

    let database_url = "mysql://rest_user:password123@mysql/rest_db_rust";

    let pool = create_pool(database_url).await?;

    app::run(pool).await
}

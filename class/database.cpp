
#include "database.h"
typedef std::string VALUEERR;

//=============class DataBase===============
DataBase::DataBase(std::string call){
  DataBase::conn.reset(new pqxx::connection(call));
  DataBase::T.reset(new pqxx::work(*DataBase::conn));
}

DataBase::~DataBase(){
  DataBase::T.get()->commit();
}

int DataBase::insertValue(Consumer c){
  try{
    if(c.getNode_ID().empty()){
      VALUEERR err = "no nodeid.";
      throw err;
    }
    else if(c.getUser_ID().empty()){
      VALUEERR err = "no userid.";
      throw err;
    }
    else if(c.getPrivacy_lvl()>MAXLVL || c.getPrivacy_lvl()<MINLVL){
      VALUEERR err = "no privacy.";
      throw err;
    }
    else if(c.getNode_Type().empty()){
      VALUEERR err = "no nodetype.";
      throw err;
    }
    else if(c.getData_Type().empty()){
      VALUEERR err = "no datatype.";
      throw err;
    }
    else if(c.getinterval()==0){
      VALUEERR err = "interval";
      throw err;
    }
    pqxx::result result=T.get()->exec(
        "SELECT * "
        "FROM node "
        "WHERE nodeid =" +T.get()->quote(c.getNode_ID()));

    if(result.size() != 0){
      std::cerr<<c.getNode_ID()<<" is existing."<<std::endl;
      return 1;
    }
    else{
      T.get()->exec(
          "INSERT INTO node "
          "VALUES (" + T.get()->quote(c.getNode_ID()) + "," + T.get()->quote(c.getUser_ID()) + "," + T.get()->quote(c.getPrivacy_lvl()) + "," +T.get()->quote(c.getNode_Type()) + "," + T.get()->quote(c.getData_Type()) + "," + std::to_string(c.getinterval()) + ");");
      return 0;
    }
  }
  catch(VALUEERR e){
    std::cerr<<e<<std::endl;
    return 1;
  }
  catch(const pqxx::sql_error& e){
    std::cerr<<e.what()<<": SQL->"<<"select id from test"<<std::endl;
    return 1;
  }
  catch(const pqxx::usage_error& e){
    std::cerr<<e.what()<<std::endl;
    return 1;
  }
}


int DataBase::insertValue(Vender v){
  try{
    if(v.getService_ID().empty()){
      VALUEERR err = "no serviceid.";
      throw err;
    }
    else if(v.getVender_ID().empty()){
      VALUEERR err = "no venderid.";
      throw err;
    }
    else if(v.getPrivacy_lvl()>MAXLVL || v.getPrivacy_lvl()<MINLVL){
      VALUEERR err = "no privacy.";
      throw err;
    }
    else if(v.getData_Type().empty()){
      VALUEERR err = "no servicetype.";
      throw err;
    }
    else if(v.getinterval()==0){
      VALUEERR err = "interval";
      throw err;
    }

    pqxx::result result=T.get()->exec(
        "SELECT * "
        "FROM service "
        "WHERE serviceid =" +T.get()->quote(v.getService_ID()));

    if(result.size() != 0){
      std::cerr<<v.getService_ID()<<" is existing."<<std::endl;
      return 1;
    }
    else{
      T.get()->exec(
          "INSERT INTO service "
          "VALUES (" + T.get()->quote(v.getService_ID()) + "," + T.get()->quote(v.getVender_ID()) + "," + T.get()->quote(v.getPrivacy_lvl()) + "," +T.get()->quote(v.getData_Type()) + "," + std::to_string(v.getinterval()) + ")");
      return 0;
    }
  }
  catch(VALUEERR e){
    std::cerr<< e <<std::endl;
    return 1;
  }
  catch(const pqxx::sql_error& e){
    std::cerr<<e.what()<<": SQL->"<<"select id from test"<<std::endl;
    return 1;
  }
  catch(const pqxx::usage_error& e){
    std::cerr<<e.what()<<std::endl;
    return 1;
  }
}



int DataBase::insertValue(Relation r){
  try{
    if(r.getAnonymization().empty()){
      VALUEERR err = "no anonymous.";
      throw err;
    }
    else if(r.getPrivacy_lvl()>MAXLVL || r.getPrivacy_lvl()<MINLVL){
      VALUEERR err = "no privacy.";
      throw err;
    }
    else if(r.getNode_ID().empty()){
      VALUEERR err = "no node.";
      throw err;
    }
    else if(r.getService_ID().empty()){
      VALUEERR err = "no service.";
      throw err;
    }
    else if(r.getinterval()==0){
      VALUEERR err = "interval";
      throw err;
    }
  }
  catch (VALUEERR e){
    std::cerr<< e << std::endl;
    return 1;
  }
  try{
    pqxx::result result=T.get()->exec(
        "SELECT relationid "
        "FROM relation "
        "WHERE nodeid = " + T.get()->quote(r.getNode_ID()) + " "
        "AND serviceid = " + T.get()->quote(r.getService_ID()));
    if(!result.empty()){
      std::cout<<result.begin()["relationid"].as<int>()<<" is exist"<<std::endl;
      return 1;
    }
    result=T.get()->exec(
        "SELECT coalesce(max(relationid), 0) +1 AS gap "
        "FROM (SELECT relationid, Row_Number() OVER(order by relationid) AS Rn FROM relation) a "
        "WHERE relationid = Rn");
    pqxx::result::iterator tmp;
    tmp=result.begin();
    r.setRelation_ID(tmp["gap"].as<int>());

    //std::string strnode="{";
    //std::vector<Node_ID> nodes=r.getNodes();
    //int nodenum=0;
    //for(std::vector<Node_ID>::iterator nodeit=nodes.begin();nodeit!=nodes.end();nodeit++){
    //  strnode+=nodes[nodenum];
    //  if(nodeit+1!=nodes.end())strnode+=",";
    //  nodenum++;
    //}
    //strnode+="}";

    //std::string strservice="{";
    //std::vector<Service_ID> services=r.getServices();
    //int servicenum=0;
    //for(std::vector<Service_ID>::iterator serviceit=services.begin();serviceit!=services.end();serviceit++){
    //  strservice+=serviceit[servicenum];
    //  if(serviceit+1!=services.end())strservice+=",";
    //  servicenum++;
    //}
    //strservice+="}";

    //std::cout<<strnode<<strservice<<std::endl;


    T.get()->exec(
        "INSERT INTO relation "
        "VALUES (" + std::to_string(r.getRelation_ID()) + "," + T.get()->quote(r.getService_ID()) + "," + T.get()->quote(r.getNode_ID()) + "," + std::to_string(r.getPrivacy_lvl()) + "," + T.get()->quote(r.getAnonymization()) + "," + std::to_string(r.getinterval()) + ")");
    return 0;
  }
  catch(const pqxx::sql_error& e){
    std::cerr<<e.what()<<": SQL->"<<"select id from test"<<std::endl;
    return -1;
  }
  catch(const pqxx::usage_error& e){
    std::cerr<<e.what()<<std::endl;
    return -1;
  }
}



int DataBase::deleteValue(std::string kind, std::string id){
  try{
    if(id.empty()){
      VALUEERR err = "no id.";
      throw err;
    }
  }
  catch (VALUEERR e){
    std::cerr<< e << std::endl;
    return 1;
  }

  try{
    if(kind=="node"){
      T.get()->exec(
          "DELETE "
          "FROM node "
          "WHERE nodeid =" +T.get()->quote(id));
     return 0;
    }
    if(kind=="service"){
      T.get()->exec(
          "DELETE "
          "FROM service "
          "WHERE serviceid =" + T.get()->quote(id));
      return 0;
    }
  }
  catch(const pqxx::sql_error& e){
    std::cerr<<e.what()<<": SQL->"<<"select id from test"<<std::endl;
    return -1;
  }
  catch(const pqxx::usage_error& e){
    std::cerr<<e.what()<<std::endl;
    return -1;
  }
}



int DataBase::deleteRelation(Node_ID nodeid, Service_ID serviceid){
  try{
    if(nodeid.empty()){
      VALUEERR err = "no nodeid.";
      throw err;
    }
    else if(serviceid.empty()){
      VALUEERR err = "no serviceid.";
      throw err;
    }
  }
  catch (VALUEERR e){
    std::cerr<< e << std::endl;
    return 1;
  }
  try{
    T.get()->exec(
        "DELETE "
        "FROM relation "
        "WHERE serviceid = " + T.get()->quote(serviceid) + " AND nodeid = " + T.get()->quote(nodeid));
    return 0;
  }
  catch(const pqxx::sql_error& e){
    std::cerr<<e.what()<<": SQL->"<<"select id from test"<<std::endl;
    return -1;
  }
  catch(const pqxx::usage_error& e){
    std::cerr<<e.what()<<std::endl;
    return -1;
  }
 
}

int DataBase::selectValue(std::string req, std::vector<Consumer> &c_res){
  try{
    pqxx::result r=T.get()->exec(
        "SELECT * "
        "FROM node "
        "WHERE " + req);
    for(pqxx::result::iterator it=r.begin();it!=r.end();it++){
      Consumer tmp;
      tmp.setNode_ID(it["nodeid"].as<std::string>());
      tmp.setUser_ID(it["userid"].as<std::string>());
      tmp.setPrivacy_lvl(it["privacy_lvl"].as<int>());
      tmp.setNode_Type(it["node_type"].as<std::string>());
      tmp.setData_Type(it["data_type"].as<std::string>());
      tmp.setinterval(it["interval"].as<int>());

      c_res.push_back(tmp);
    }
    return 0;
  }
  catch(std::string e){
    std::cerr<<e<<std::endl;
  }
  catch(const pqxx::sql_error& e){
    std::cerr<<e.what()<<": SQL->"<<"select id from test"<<std::endl;
    return -1;
  }
  catch(const pqxx::usage_error& e){
    std::cerr<<e.what()<<std::endl;
    return -1;
  }
}



int DataBase::selectValue(std::string req, std::vector<Vender> &v_res){
  try{
    pqxx::result r=T.get()->exec(
        "SELECT * "
        "FROM service "
        "WHERE " + req);
    for(pqxx::result::iterator it=r.begin();it!=r.end();it++){
      Vender tmp;
      tmp.setService_ID(it["serviceid"].as<std::string>());
      tmp.setVender_ID(it["venderid"].as<std::string>());
      tmp.setPrivacy_lvl(it["privacy_lvl"].as<int>());
      tmp.setData_Type(it["data_type"].as<std::string>());
      tmp.setinterval(it["interval"].as<int>());
      v_res.push_back(tmp);
    }
    return 0;
  }
  catch(std::string e){
    std::cerr<<e<<std::endl;
  }
  catch(const pqxx::sql_error& e){
    std::cerr<<e.what()<<": SQL->"<<"select id from test"<<std::endl;
    return -1;
  }
  catch(const pqxx::usage_error& e){
    std::cerr<<e.what()<<std::endl;
    return -1;
  }
}



int DataBase::selectValue(std::string req, std::vector<Relation> &r_res){
  try{
    pqxx::result r=T.get()->exec(
        "SELECT * "
        "FROM relation "
        "WHERE " + req);
    for(pqxx::result::iterator it=r.begin();it!=r.end();it++){
      Relation tmp;
      //std::vector<Node_ID> nodes;
      //std::vector<Service_ID> services;
      tmp.setRelation_ID(it["relationid"].as<int>());
      //nodes=splitarray(it["nodeid"].as<std::string>());
      tmp.setNode_ID(it["nodeid"].as<std::string>());
      //for(int i = 0;i!=nodes.size();i++){
      //  tmp.addNodes(nodes[i]);
      //  std::cout<<nodes[i]<<std::endl;
      //}
      //services=splitarray(it["serviceid"].as<std::string>());
      tmp.setService_ID(it["serviceid"].as<std::string>());
      //for(int i = 0; i!=services.size();i++){
      //  tmp.addServices(services[i]);
      //  std::cout<<services[i]<<std::endl;
      //}
      tmp.setAnonymization(it["anonymization_method"].as<std::string>());
      tmp.setPrivacy_lvl(it["privacy_lvl"].as<int>());
      tmp.setinterval(it["interval"].as<int>());
      r_res.push_back(tmp);
    }
    return 0;
  }
  catch(std::string e){
    std::cerr<<e<<std::endl;
  }
  catch(const pqxx::sql_error& e){
    std::cerr<<e.what()<<": SQL->"<<"select id from test"<<std::endl;
    return -1;
  }
  catch(const pqxx::usage_error& e){
    std::cerr<<e.what()<<std::endl;
    return -1;
  }
}

std::string DataBase::quote(std::string in){
  return T.get()->quote(in);
}

int DataBase::updataRelationPrivacy(Node_ID nodeid, Service_ID serviceid, int lvl){
  try{
    pqxx::result result=T.get()->exec("SELECT relationid FROM relation WHERE nodeid = " + T.get()->quote(nodeid) + "AND serviceid = " + T.get()->quote(serviceid));
    if(result.empty())return 1;
    T.get()->exec("UPDATE relation SET privacy_lvl = " + std::to_string(lvl) + " WHERE relationid = " + result.begin()["relationid"].as<std::string>());
    return 0;
  }
  catch(const pqxx::sql_error& e){
    std::cerr<<e.what()<<": SQL->"<<"select relationid from relation"<<std::endl;
    return -1;
  }
  catch(const pqxx::usage_error& e){
    std::cerr<<e.what()<<std::endl;
    return -1;
  }
  catch(...){
    std::cerr<<"err\n";
  }
}

bool DataBase::authUser(std::string username,std::string password, std::string &userid){
  try{
    pqxx::result result=T.get()->exec("SELECT * FROM users WHERE username = " + T.get()->quote(username));
    if(result.empty())return false;
    else{
      std::string salt;
      pqxx::result::iterator itr=result.begin();
      salt=itr["salt"].as<std::string>();
      if(itr["password"].as<std::string>()==mysha256(salt+password)){
        userid=itr["userid"].as<std::string>();
        return true;
      }else return false;
    }
  }
  catch(const pqxx::sql_error& e){
    std::cerr<<e.what()<<": SQL->"<<"select relationid from relation"<<std::endl;
    return false;
  }
  catch(const pqxx::usage_error& e){
    std::cerr<<e.what()<<std::endl;
    return false;
  }
  catch(...){
    std::cerr<<"err\n";
    return false;
  }
}

bool DataBase::authVender(std::string vendername,std::string password, std::string &venderid){
  try{
    pqxx::result result=T.get()->exec("SELECT * FROM venders WHERE vendername = " + T.get()->quote(vendername));
    if(result.empty())return false;
    else{
      std::string salt;
      pqxx::result::iterator itr=result.begin();
      salt=itr["salt"].as<std::string>();
      if(itr["password"].as<std::string>()==mysha256(salt+password)){
        venderid=itr["venderid"].as<std::string>();
        return true;
      }else return false;
    }
  }
  catch(const pqxx::sql_error& e){
    std::cerr<<e.what()<<": SQL->"<<"select relationid from relation"<<std::endl;
    return false;
  }
  catch(const pqxx::usage_error& e){
    std::cerr<<e.what()<<std::endl;
    return false;
  }
  catch(...){
    std::cerr<<"err\n";
    return false;
  }
}

bool DataBase::insertUser(std::string username, std::string password){
  try{
    std::string salt;
    salt="testsalt";
    std::string hashedpass;
    hashedpass=mysha256("testsalt"+password);
    std::string userid="u00001";
    T.get()->exec("INSERT INTO users "
        "VALUES (" + T.get()->quote(username) +
        ", " + T.get()->quote(hashedpass) +
        ", " + T.get()->quote(salt) +
        ", " + T.get()->quote(userid) + ")");
    return true;
  }
  catch(const pqxx::sql_error& e){ std::cerr<<e.what()<<": SQL->"<<"select relationid from relation"<<std::endl;
    return -1;
  }
  catch(const pqxx::usage_error& e){
    std::cerr<<e.what()<<std::endl;
    return -1;
  }
  catch(...){
    std::cerr<<"err\n";
  }
}

//==========================================


//===============splitarray=================
std::vector<std::string> splitarray(std::string str){
  size_t c;
  while((c=str.find_first_of(" {}")) != std::string::npos){
      str.erase(c,1);
  }
  return split(str,',');
}
//==========================================

//================string split===================
std::vector<std::string> split(const std::string &str, char sep){
  std::vector<std::string> v;
  std::stringstream ss(str);
  std::string buffer;
  while( std::getline(ss, buffer, sep) ){
    v.push_back(buffer);
  }
  return v;
}
//===============================================
int main(){
  DataBase db("dbname=test user=testuser password=testpass");
//
//  Consumer c[10];
//  for(int i=0;i<10;i++){
//  c[i].setNode_ID("http://GW/sensor"+std::to_string(i));
//  c[i].setUser_ID("jhon");
//  if(i%3==0)  c[i].setPrivacy_lvl(1);
//  else if(i%3==1) c[i].setPrivacy_lvl(2);
//  else c[i].setPrivacy_lvl(3);
//  //if(i%2==0) c[i].setNode_Type("sensor");
//  //else c[i].setNode_Type("actuator");
//  c[i].setNode_Type("sensor");
//  if(i%2==0) c[i].setData_Type("power");
//  else c[i].setData_Type("temp");
//  c[i].setinterval((i+1)*60);
//  db.deleteValue("node",c[i].getNode_ID());
//  db.insertValue(c[i]);
//  }
//  std::vector<Consumer> res;
//  db.selectValue("privacy_lvl >= 2",res);
//
//  for(std::vector<Consumer>::iterator it=res.begin();it!=res.end();it++){
//    std::cout<<it->getNode_ID()<<it->getUser_ID()<<it->getPrivacy_lvl()<<it->getNode_Type()<<it->getData_Type()<<it->getinterval()<<std::endl;
//  }
//
//  Vender v[10];
//  for(int i=0;i<10;i++){
//    v[i].setService_ID("http://service/dummy"+std::to_string(i));
//    v[i].setVender_ID("WWW Co.");
//    if(i%3==0) v[i].setPrivacy_lvl(3);
//    else if(i%3==1) v[i].setPrivacy_lvl(2);
//    else v[i].setPrivacy_lvl(1);
//    if(i%2==0) v[i].setData_Type("temp");
//    else v[i].setData_Type("power");
//    v[i].setinterval((i+1)*60);
//    db.deleteValue("service",v[i].getService_ID());
//    db.insertValue(v[i]);
//  }
//  std::vector<Vender> vres;
//  db.selectValue("privacy_lvl > 1",vres);
//for(std::vector<Vender>::iterator it=vres.begin();it!=vres.end();it++){
//    std::cout<<it->getService_ID()<<it->getVender_ID()<<it->getPrivacy_lvl()<<it->getData_Type()<<it->getinterval()<<std::endl;
//  }
//
//  Relation r;
//  r.setNode_ID("http://GW/sensor2");
//  r.setService_ID("http://service/dummy1");
//  r.setAnonymization("noise");
//  r.setPrivacy_lvl(3);
//  r.setinterval(180);
//  db.insertValue(r);
//  std::cout<<"ok\n";
//
//  std::vector<Relation> r_res;
//  db.selectValue("relationid=1",r_res);
//  for(std::vector<Relation>::iterator it=r_res.begin();it!=r_res.end();it++){
//    std::cout<<it->getRelation_ID()<<std::endl;
//    std::cout<<it->getNode_ID()<<std::endl;
//    std::cout<<it->getService_ID()<<std::endl;
//    std::cout<<it->getinterval()<<std::endl;
//
//    //std::vector<Node_ID> nodesres=it->getNodes();
//    //for(std::vector<Node_ID>::iterator nodeit=nodesres.begin();nodeit!=nodesres.end();nodeit++){
//    //  std::cout<<*nodeit<<std::endl;
//    //}
//
//    //std::vector<Service_ID> servicesres=it->getServices();
//    //for(std::vector<Service_ID>::iterator serviceit=servicesres.begin();serviceit!=servicesres.end();serviceit++){
//    //  std::cout<<*serviceit<<std::endl;
//    //}
//
//    std::cout<<it->getAnonymization()<<it->getPrivacy_lvl()<<std::endl;
//  }
//  //db.deleteRelation("'jklsdjl'=ANY(nodeid)");
//  return 0;
//
  //db.insertUser("jhon","testpass");
  std::string userid;
  if(db.authUser("jhon","testpass",userid)){
    std::cout<<userid<<std::endl;
  }
  else std::cerr<<"cant";
  if(db.authUser("jhon","notpass",userid)){
    std::cout<<userid<<std::endl;
  }
  else std::cerr<<"cant";
  if(db.authUser("allry","testpass",userid)){
    std::cout<<userid<<std::endl;
  }
  else std::cerr<<"cant";


  return 0;
}



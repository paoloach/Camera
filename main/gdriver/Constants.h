//
// Created by paolo on 05/12/19.
//

#pragma once

constexpr char CLIENT_ID[] = "client_id";
constexpr uint16_t CLIENT_ID_LEN = sizeof(CLIENT_ID)-1;
constexpr char SCOPE[]="scope";
constexpr uint16_t SCOPE_LEN= sizeof(SCOPE)-1;
constexpr char PROFILE[]="https://www.googleapis.com/auth/drive.file%20profile";
constexpr uint16_t PROFILE_LEN=sizeof(PROFILE)-1;
constexpr char CLIENT_SECRET[] ="client_secret";
constexpr uint16_t CLIENT_SECRET_LEN=sizeof(CLIENT_SECRET)-1;
constexpr char CODE[] ="code";
constexpr uint16_t CODE_LEN=sizeof(CODE)-1;
constexpr char GRANT[]="grant_type";
constexpr uint16_t GRANT_LEN=sizeof(GRANT)-1;
constexpr char GRANT_TYPE[]="http://oauth.net/grant_type/device/1.0";
constexpr uint16_t GRANT_TYPE_LEN=sizeof(GRANT_TYPE)-1;
constexpr char CONTENT_TYPE[] = "Content-Type";
constexpr uint16_t CONTENT_TYPE_LEN=sizeof(CONTENT_TYPE)-1;
constexpr char CONTENT_TYPE_FORM[]="application/x-www-form-urlencoded";
constexpr uint16_t CONTENT_TYPE_FORM_LEN=sizeof(CONTENT_TYPE_FORM)-1;
constexpr char REFRESH_TOKEN[] = "refresh_token";
constexpr uint16_t REFRESH_TOKEN_LEN = sizeof(REFRESH_TOKEN)-1;
constexpr char AUTHORIZATION_HEADER[] = "Authorization";
constexpr uint16_t AUTHORIZATION_HEADER_LEN = sizeof(AUTHORIZATION_HEADER)-1;;
constexpr char BEARER_HEADER[] = "Bearer ";
constexpr uint16_t BEARER_HEADER_LEN = sizeof(BEARER_HEADER)-1;;
constexpr char FILTER_FOR_FOLDER[]="q";
constexpr char FILTER_FOR_FOLDER_LEN= sizeof(FILTER_FOR_FOLDER)-1;
constexpr char FILTER_FOR_FOLDER_VALUE[]= "mimeType = 'application/vnd.google-apps.folder' and name='serra'";
constexpr char FILTER_FOR_FOLDER_VALUE_LEN= sizeof(FILTER_FOR_FOLDER_VALUE)-1;
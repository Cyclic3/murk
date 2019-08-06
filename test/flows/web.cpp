#include <gtest/gtest.h>

#include "murk/flows/web.hpp"

TEST(webUri, render) {
  murk::web::uri uri;
  uri.scheme = "http";

  uri.auth.emplace();
  uri.auth->host = "example.com";
  uri.auth->port = "80";

  uri.path().components = {"foo", "bar", "baz"};
  uri.path().is_dir = false;

  uri.query() = "qux";
  uri.fragment() = "wibble";
  ASSERT_EQ(uri.render(), "http://example.com:80/foo/bar/baz?qux#wibble");
}

TEST(webUri, parse) {
  murk::web::uri uri = murk::web::uri::parse("http://toor@example.com:80/foo/bar/baz?qux#wibble");
  murk::web::uri test;

  EXPECT_EQ(uri.scheme, "http");

  EXPECT_TRUE(uri.auth.has_value());

  if (uri.auth) {
    EXPECT_TRUE(uri.auth->userinfo.has_value());
    if (uri.auth->userinfo)
      EXPECT_EQ(*uri.auth->userinfo, "toor");
    EXPECT_EQ(uri.auth->host, "example.com");
    EXPECT_TRUE(uri.auth->port.has_value());
    if (uri.auth->port)
      EXPECT_EQ(*uri.auth->port, "80");
  }

  murk::web::uri::stem expected_path;
  expected_path.components = {"foo", "bar", "baz"};
  expected_path.is_dir = false;
  EXPECT_EQ(uri.path().components, expected_path.components);
  EXPECT_EQ(uri.path().is_dir, expected_path.is_dir);

  EXPECT_EQ(uri.query(), "qux");
  EXPECT_EQ(uri.fragment(), "wibble");

  EXPECT_EQ(uri.render(), "http://toor@example.com:80/foo/bar/baz?qux#wibble");

  uri.path().is_dir = true;
  EXPECT_EQ(uri.render(), "http://toor@example.com:80/foo/bar/baz/?qux#wibble");
  test = murk::web::uri::parse("http://toor@example.com:80/foo/bar/baz/?qux#wibble");
  EXPECT_EQ(test, uri);
  EXPECT_EQ(test.render(), "http://toor@example.com:80/foo/bar/baz/?qux#wibble");
}

//TEST(web, download) {
//  std::string uri = "http://localhost:8080/";
//  ASSERT_EQ(murk::web::uri_t::parse(uri).render(), uri);
//  ASSERT_EQ(murk::web::download(uri), "");
//}

// 获取DEF云构建publicPath，多用于dynamic import需要指定输出publicPath
import yargsParser from 'yargs-parser';

function getPublicPath() {
  let publicPath = '/';

  const { BUILD_ARGV_STR, BUILD_GIT_GROUP, BUILD_GIT_PROJECT, BUILD_GIT_BRANCH } = process.env;


  let publishType;
  let publishEnv;

  if (BUILD_ARGV_STR) {
    const buildArgv = yargsParser(BUILD_ARGV_STR);
    publishType = buildArgv.def_publish_type;
    publishEnv = buildArgv.def_publish_env;

    if (publishType) {
      const cdnHost = publishEnv === 'daily' ? '//dev.g.alicdn.com' : '//g.alicdn.com';
      publicPath = `${cdnHost}/${BUILD_GIT_GROUP}/${BUILD_GIT_PROJECT}/${BUILD_GIT_BRANCH.split('/')[1]}/`;
    }
  } else {
    return '/';
  }

  return publicPath;
}

export default getPublicPath;
